/**
 * @file noise.tcc
 *
 * Copyright (c) 2026 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 *
 * Based on OpenSimplex2S by Kurt Spencer (public domain).
 * See: https://github.com/KdotJPG/OpenSimplex2
 */

#include "noise.h"

#ifndef CORTEX_NOISE_TCC
#define CORTEX_NOISE_TCC

#include <array>
#include <cmath>

namespace {

// Hash primes and multiplier (from reference)
static const long long kPrimeX        = 0x5205402B9270C86FLL;
static const long long kPrimeY        = 0x598CD327003817B5LL;
static const long long kPrimeZ        = 0x5BCC226E9FA0BACBLL;
static const long long kHashMultiplier = 0x53A3F72DEEC546F5LL;
static const long long kSeedFlip3D    = -0x52D547B2E96ED629LL;
// Doubled primes (computed via unsigned to avoid signed overflow)
static const long long kPrimeX2 = (long long)((unsigned long long)kPrimeX * 2ULL);
static const long long kPrimeY2 = (long long)((unsigned long long)kPrimeY * 2ULL);
static const long long kPrimeZ2 = (long long)((unsigned long long)kPrimeZ * 2ULL);

// Normalizers (pre-divide gradients to keep output in [-1, 1])
static const double kNormalizer2D = 0.05481866495625118;
static const double kNormalizer3D = 0.2781926117527186;

// 24 unit gradient vectors for 2D (8 at 45° + 16 at 15° intervals)
static const double kGrad2Raw[48] = {
     0.38268343236509,   0.923879532511287,
     0.923879532511287,  0.38268343236509,
     0.923879532511287, -0.38268343236509,
     0.38268343236509,  -0.923879532511287,
    -0.38268343236509,  -0.923879532511287,
    -0.923879532511287, -0.38268343236509,
    -0.923879532511287,  0.38268343236509,
    -0.38268343236509,   0.923879532511287,
     0.130526192220052,  0.99144486137381,
     0.608761429008721,  0.793353340291235,
     0.793353340291235,  0.608761429008721,
     0.99144486137381,   0.130526192220051,
     0.99144486137381,  -0.130526192220051,
     0.793353340291235, -0.60876142900872,
     0.608761429008721, -0.793353340291235,
     0.130526192220052, -0.99144486137381,
    -0.130526192220052, -0.99144486137381,
    -0.608761429008721, -0.793353340291235,
    -0.793353340291235, -0.608761429008721,
    -0.99144486137381,  -0.130526192220052,
    -0.99144486137381,   0.130526192220051,
    -0.793353340291235,  0.608761429008721,
    -0.608761429008721,  0.793353340291235,
    -0.130526192220052,  0.99144486137381,
};

// 48 BCC-lattice gradient vectors for 3D, stride-4 (4th element = 0)
static const double kGrad3Raw[192] = {
     2.22474487139,      2.22474487139,     -1.0,                0.0,
     2.22474487139,      2.22474487139,      1.0,                0.0,
     3.0862664687972017, 1.1721513422464978, 0.0,                0.0,
     1.1721513422464978, 3.0862664687972017, 0.0,                0.0,
    -2.22474487139,      2.22474487139,     -1.0,                0.0,
    -2.22474487139,      2.22474487139,      1.0,                0.0,
    -1.1721513422464978, 3.0862664687972017, 0.0,                0.0,
    -3.0862664687972017, 1.1721513422464978, 0.0,                0.0,
    -1.0,               -2.22474487139,     -2.22474487139,      0.0,
     1.0,               -2.22474487139,     -2.22474487139,      0.0,
     0.0,               -3.0862664687972017,-1.1721513422464978, 0.0,
     0.0,               -1.1721513422464978,-3.0862664687972017, 0.0,
    -1.0,               -2.22474487139,      2.22474487139,      0.0,
     1.0,               -2.22474487139,      2.22474487139,      0.0,
     0.0,               -1.1721513422464978, 3.0862664687972017, 0.0,
     0.0,               -3.0862664687972017, 1.1721513422464978, 0.0,
    -2.22474487139,     -2.22474487139,     -1.0,                0.0,
    -2.22474487139,     -2.22474487139,      1.0,                0.0,
    -3.0862664687972017,-1.1721513422464978, 0.0,                0.0,
    -1.1721513422464978,-3.0862664687972017, 0.0,                0.0,
    -2.22474487139,     -1.0,               -2.22474487139,      0.0,
    -2.22474487139,      1.0,               -2.22474487139,      0.0,
    -1.1721513422464978, 0.0,               -3.0862664687972017, 0.0,
    -3.0862664687972017, 0.0,               -1.1721513422464978, 0.0,
    -2.22474487139,     -1.0,                2.22474487139,      0.0,
    -2.22474487139,      1.0,                2.22474487139,      0.0,
    -3.0862664687972017, 0.0,                1.1721513422464978, 0.0,
    -1.1721513422464978, 0.0,                3.0862664687972017, 0.0,
    -1.0,                2.22474487139,     -2.22474487139,      0.0,
     1.0,                2.22474487139,     -2.22474487139,      0.0,
     0.0,                1.1721513422464978,-3.0862664687972017, 0.0,
     0.0,                3.0862664687972017,-1.1721513422464978, 0.0,
    -1.0,                2.22474487139,      2.22474487139,      0.0,
     1.0,                2.22474487139,      2.22474487139,      0.0,
     0.0,                3.0862664687972017, 1.1721513422464978, 0.0,
     0.0,                1.1721513422464978, 3.0862664687972017, 0.0,
     2.22474487139,     -2.22474487139,     -1.0,                0.0,
     2.22474487139,     -2.22474487139,      1.0,                0.0,
     1.1721513422464978,-3.0862664687972017, 0.0,                0.0,
     3.0862664687972017,-1.1721513422464978, 0.0,                0.0,
     2.22474487139,     -1.0,               -2.22474487139,      0.0,
     2.22474487139,      1.0,               -2.22474487139,      0.0,
     3.0862664687972017, 0.0,               -1.1721513422464978, 0.0,
     1.1721513422464978, 0.0,               -3.0862664687972017, 0.0,
     2.22474487139,     -1.0,                2.22474487139,      0.0,
     2.22474487139,      1.0,                2.22474487139,      0.0,
     1.1721513422464978, 0.0,                3.0862664687972017, 0.0,
     3.0862664687972017, 0.0,                1.1721513422464978, 0.0,
};

// Gradient tables: 256 entries (2D, 128 pairs) and 1024 entries (3D, 256 quads),
// each cycling the raw arrays and pre-dividing by the normalizer.
static const double* grad2Table()
{
    static const std::array<double, 256> kTable = []() {
        std::array<double, 256> t{};
        for (int i = 0; i < 256; ++i)
            t[i] = kGrad2Raw[i % 48] / kNormalizer2D;
        return t;
    }();
    return kTable.data();
}

static const double* grad3Table()
{
    static const std::array<double, 1024> kTable = []() {
        std::array<double, 1024> t{};
        for (int i = 0; i < 1024; ++i)
            t[i] = kGrad3Raw[i % 192] / kNormalizer3D;
        return t;
    }();
    return kTable.data();
}

// Dot product of a normalized 2D gradient with displacement (dx, dy).
// hash selects the gradient; table has 128 pairs (256 doubles), mask = 254.
inline double grad2(long long seed, long long xvp, long long yvp, double dx, double dy)
{
    long long hash = seed ^ xvp ^ yvp;
    hash *= kHashMultiplier;
    hash ^= (long long)((unsigned long long)hash >> 58);
    const int gi = (int)hash & 254;
    const double* g = grad2Table();
    return g[gi] * dx + g[gi + 1] * dy;
}

// Dot product of a normalized 3D gradient with displacement (dx, dy, dz).
// hash selects the gradient; table has 256 quads (1024 doubles), mask = 1020.
inline double grad3(long long seed, long long xvp, long long yvp, long long zvp,
                    double dx, double dy, double dz)
{
    long long hash = (seed ^ xvp) ^ (yvp ^ zvp);
    hash *= kHashMultiplier;
    hash ^= (long long)((unsigned long long)hash >> 58);
    const int gi = (int)hash & 1020;
    const double* g = grad3Table();
    return g[gi] * dx + g[gi + 1] * dy + g[gi + 2] * dz;
}

inline int fastFloor(double x)
{
    const int xi = (int)x;
    return x < xi ? xi - 1 : xi;
}

} // anonymous namespace

template <typename T>
OpenSimplex2S<T>::OpenSimplex2S(long long seed)
    : seed_(seed)
{
}

template <typename T>
T OpenSimplex2S<T>::noise(T x, T y) const
{
    // Geometry constants
    static const double kSkew   =  0.366025403784439;    // (sqrt(3)-1)/2
    static const double kUnskew = -0.211324865405187;    // UNSKEW_2D
    static const double kRsq    =  2.0 / 3.0;           // RSQUARED_2D
    // Derived: d = 1 + 2*UNSKEW_2D (displacement to (1,1) vertex in unskewed space)
    static const double kD      =  1.0 + 2.0 * kUnskew; // ≈ 0.5774 = ROOT3OVER3
    // Coefficient for the algebraic shortcut computing a1 from a0 and t
    static const double kA1coeff = 2.0 * kD * (1.0 / kUnskew + 2.0);
    static const double kA1base  = -2.0 * kD * kD;

    const double xd = x, yd = y;

    // Skew input to triangular lattice
    const double s  = kSkew * (xd + yd);
    const double xs = xd + s, ys = yd + s;

    // Base cell
    const int xsb = fastFloor(xs), ysb = fastFloor(ys);
    const double xi = xs - xsb, yi = ys - ysb;

    // Prime-multiplied coordinates for hashing
    const long long xsbp = (long long)xsb * kPrimeX;
    const long long ysbp = (long long)ysb * kPrimeY;

    // Unskew to get displacements to vertex (0,0)
    const double t   = (xi + yi) * kUnskew;
    const double dx0 = xi + t, dy0 = yi + t;

    // Vertex (0,0): always contributes (a0 >= 0 for all xi,yi in [0,1))
    const double a0 = kRsq - dx0 * dx0 - dy0 * dy0;
    double value = (a0 * a0) * (a0 * a0) * grad2(seed_, xsbp, ysbp, dx0, dy0);

    // Vertex (1,1): always contributes; falloff derived algebraically from a0 and t
    const double a1  = kA1coeff * t + (kA1base + a0);
    const double dx1 = dx0 - kD, dy1 = dy0 - kD;
    value += (a1 * a1) * (a1 * a1) * grad2(seed_, xsbp + kPrimeX, ysbp + kPrimeY, dx1, dy1);

    // Two conditional extra vertices selected based on which simplex half we're in
    const double xmyi = xi - yi;
    if (t < kUnskew) {
        if (xi + xmyi > 1.0) {
            const double dx2 = dx0 - (3.0 * kUnskew + 2.0);
            const double dy2 = dy0 - (3.0 * kUnskew + 1.0);
            const double a2  = kRsq - dx2 * dx2 - dy2 * dy2;
            if (a2 > 0)
                value += (a2 * a2) * (a2 * a2) * grad2(seed_, xsbp + kPrimeX2, ysbp + kPrimeY, dx2, dy2);
        } else {
            const double dx2 = dx0 - kUnskew;
            const double dy2 = dy0 - (kUnskew + 1.0);
            const double a2  = kRsq - dx2 * dx2 - dy2 * dy2;
            if (a2 > 0)
                value += (a2 * a2) * (a2 * a2) * grad2(seed_, xsbp, ysbp + kPrimeY, dx2, dy2);
        }
        if (yi - xmyi > 1.0) {
            const double dx3 = dx0 - (3.0 * kUnskew + 1.0);
            const double dy3 = dy0 - (3.0 * kUnskew + 2.0);
            const double a3  = kRsq - dx3 * dx3 - dy3 * dy3;
            if (a3 > 0)
                value += (a3 * a3) * (a3 * a3) * grad2(seed_, xsbp + kPrimeX, ysbp + kPrimeY2, dx3, dy3);
        } else {
            const double dx3 = dx0 - (kUnskew + 1.0);
            const double dy3 = dy0 - kUnskew;
            const double a3  = kRsq - dx3 * dx3 - dy3 * dy3;
            if (a3 > 0)
                value += (a3 * a3) * (a3 * a3) * grad2(seed_, xsbp + kPrimeX, ysbp, dx3, dy3);
        }
    } else {
        if (xi + xmyi < 0.0) {
            const double dx2 = dx0 + (1.0 + kUnskew);
            const double dy2 = dy0 + kUnskew;
            const double a2  = kRsq - dx2 * dx2 - dy2 * dy2;
            if (a2 > 0)
                value += (a2 * a2) * (a2 * a2) * grad2(seed_, xsbp - kPrimeX, ysbp, dx2, dy2);
        } else {
            const double dx2 = dx0 - (kUnskew + 1.0);
            const double dy2 = dy0 - kUnskew;
            const double a2  = kRsq - dx2 * dx2 - dy2 * dy2;
            if (a2 > 0)
                value += (a2 * a2) * (a2 * a2) * grad2(seed_, xsbp + kPrimeX, ysbp, dx2, dy2);
        }
        if (yi < xmyi) {
            const double dx2 = dx0 + kUnskew;
            const double dy2 = dy0 + (kUnskew + 1.0);
            const double a2  = kRsq - dx2 * dx2 - dy2 * dy2;
            if (a2 > 0)
                value += (a2 * a2) * (a2 * a2) * grad2(seed_, xsbp, ysbp - kPrimeY, dx2, dy2);
        } else {
            const double dx2 = dx0 - kUnskew;
            const double dy2 = dy0 - (kUnskew + 1.0);
            const double a2  = kRsq - dx2 * dx2 - dy2 * dy2;
            if (a2 > 0)
                value += (a2 * a2) * (a2 * a2) * grad2(seed_, xsbp, ysbp + kPrimeY, dx2, dy2);
        }
    }

    return static_cast<T>(value);
}

template <typename T>
T OpenSimplex2S<T>::noise(T x, T y, T z) const
{
    static const double kRotOrtho    = -0.211324865405187; // ROTATE3_ORTHOGONALIZER = UNSKEW_2D
    static const double kRoot3Over3  =  0.577350269189626; // ROOT3OVER3
    static const double kRsq         =  0.75;              // RSQUARED_3D

    const double xd = x, yd = y, zd = z;

    // Orthonormal rotation aligning the BCC lattice with the XY plane (ImproveXY variant)
    const double xy = xd + yd;
    const double s2 = xy * kRotOrtho;
    const double zz = zd * kRoot3Over3;
    const double xr = xd + s2 + zz;
    const double yr = yd + s2 + zz;
    const double zr = xy * (-kRoot3Over3) + zz;

    // Base cell in rotated space
    const int xrb = fastFloor(xr), yrb = fastFloor(yr), zrb = fastFloor(zr);
    const double xi = xr - xrb, yi = yr - yrb, zi = zr - zrb;

    // Prime-multiplied coordinates; second seed for body-centre BCC sublattice
    const long long xrbp  = (long long)xrb * kPrimeX;
    const long long yrbp  = (long long)yrb * kPrimeY;
    const long long zrbp  = (long long)zrb * kPrimeZ;
    const long long seed2 = seed_ ^ kSeedFlip3D;

    // xNMask = -1 when xi > 0.5 (nearest corner has +1 offset), 0 otherwise
    const int xNMask = (int)(-0.5 - xi);
    const int yNMask = (int)(-0.5 - yi);
    const int zNMask = (int)(-0.5 - zi);

    // First vertex: nearest corner of BCC sublattice 1 (always contributes)
    const double x0 = xi + xNMask, y0 = yi + yNMask, z0 = zi + zNMask;
    const double a0 = kRsq - x0 * x0 - y0 * y0 - z0 * z0;
    double value = (a0 * a0) * (a0 * a0) * grad3(seed_,
        xrbp + ((long long)xNMask & kPrimeX),
        yrbp + ((long long)yNMask & kPrimeY),
        zrbp + ((long long)zNMask & kPrimeZ),
        x0, y0, z0);

    // Second vertex: body centre of BCC sublattice 2 (always contributes)
    const double x1 = xi - 0.5, y1 = yi - 0.5, z1 = zi - 0.5;
    const double a1 = kRsq - x1 * x1 - y1 * y1 - z1 * z1;
    value += (a1 * a1) * (a1 * a1) * grad3(seed2,
        xrbp + kPrimeX, yrbp + kPrimeY, zrbp + kPrimeZ,
        x1, y1, z1);

    // Algebraic shortcuts: derive falloff at flipped vertices from a0 / a1
    const double xAFlipMask0 = ((double)((xNMask | 1) << 1)) * x1;
    const double yAFlipMask0 = ((double)((yNMask | 1) << 1)) * y1;
    const double zAFlipMask0 = ((double)((zNMask | 1) << 1)) * z1;
    const double xAFlipMask1 = ((double)(-2 - (xNMask << 2))) * x1 - 1.0;
    const double yAFlipMask1 = ((double)(-2 - (yNMask << 2))) * y1 - 1.0;
    const double zAFlipMask1 = ((double)(-2 - (zNMask << 2))) * z1 - 1.0;

    // Each of the three pairs below evaluates up to 3 conditional vertices and
    // may set a skip flag preventing a redundant evaluation in the final block.

    bool skip5 = false;
    {
        const double a2 = xAFlipMask0 + a0;
        if (a2 > 0) {
            value += (a2 * a2) * (a2 * a2) * grad3(seed_,
                xrbp + (~(long long)xNMask & kPrimeX),
                yrbp + ((long long)yNMask  & kPrimeY),
                zrbp + ((long long)zNMask  & kPrimeZ),
                x0 - (double)(xNMask | 1), y0, z0);
        } else {
            const double a3 = yAFlipMask0 + zAFlipMask0 + a0;
            if (a3 > 0) {
                value += (a3 * a3) * (a3 * a3) * grad3(seed_,
                    xrbp + ((long long)xNMask  & kPrimeX),
                    yrbp + (~(long long)yNMask & kPrimeY),
                    zrbp + (~(long long)zNMask & kPrimeZ),
                    x0, y0 - (double)(yNMask | 1), z0 - (double)(zNMask | 1));
            }
            const double a4 = xAFlipMask1 + a1;
            if (a4 > 0) {
                value += (a4 * a4) * (a4 * a4) * grad3(seed2,
                    xrbp + ((long long)xNMask & (kPrimeX2)),
                    yrbp + kPrimeY,
                    zrbp + kPrimeZ,
                    (double)(xNMask | 1) + x1, y1, z1);
                skip5 = true;
            }
        }
    }

    bool skip9 = false;
    {
        const double a6 = yAFlipMask0 + a0;
        if (a6 > 0) {
            value += (a6 * a6) * (a6 * a6) * grad3(seed_,
                xrbp + ((long long)xNMask  & kPrimeX),
                yrbp + (~(long long)yNMask & kPrimeY),
                zrbp + ((long long)zNMask  & kPrimeZ),
                x0, y0 - (double)(yNMask | 1), z0);
        } else {
            const double a7 = xAFlipMask0 + zAFlipMask0 + a0;
            if (a7 > 0) {
                value += (a7 * a7) * (a7 * a7) * grad3(seed_,
                    xrbp + (~(long long)xNMask & kPrimeX),
                    yrbp + ((long long)yNMask  & kPrimeY),
                    zrbp + (~(long long)zNMask & kPrimeZ),
                    x0 - (double)(xNMask | 1), y0, z0 - (double)(zNMask | 1));
            }
            const double a8 = yAFlipMask1 + a1;
            if (a8 > 0) {
                value += (a8 * a8) * (a8 * a8) * grad3(seed2,
                    xrbp + kPrimeX,
                    yrbp + ((long long)yNMask & (kPrimeY2)),
                    zrbp + kPrimeZ,
                    x1, (double)(yNMask | 1) + y1, z1);
                skip9 = true;
            }
        }
    }

    bool skipD = false;
    {
        const double aA = zAFlipMask0 + a0;
        if (aA > 0) {
            value += (aA * aA) * (aA * aA) * grad3(seed_,
                xrbp + ((long long)xNMask  & kPrimeX),
                yrbp + ((long long)yNMask  & kPrimeY),
                zrbp + (~(long long)zNMask & kPrimeZ),
                x0, y0, z0 - (double)(zNMask | 1));
        } else {
            const double aB = xAFlipMask0 + yAFlipMask0 + a0;
            if (aB > 0) {
                value += (aB * aB) * (aB * aB) * grad3(seed_,
                    xrbp + (~(long long)xNMask & kPrimeX),
                    yrbp + (~(long long)yNMask & kPrimeY),
                    zrbp + ((long long)zNMask  & kPrimeZ),
                    x0 - (double)(xNMask | 1), y0 - (double)(yNMask | 1), z0);
            }
            const double aC = zAFlipMask1 + a1;
            if (aC > 0) {
                value += (aC * aC) * (aC * aC) * grad3(seed2,
                    xrbp + kPrimeX,
                    yrbp + kPrimeY,
                    zrbp + ((long long)zNMask & (kPrimeZ2)),
                    x1, y1, (double)(zNMask | 1) + z1);
                skipD = true;
            }
        }
    }

    if (!skip5) {
        const double a5 = yAFlipMask1 + zAFlipMask1 + a1;
        if (a5 > 0) {
            value += (a5 * a5) * (a5 * a5) * grad3(seed2,
                xrbp + kPrimeX,
                yrbp + ((long long)yNMask & (kPrimeY2)),
                zrbp + ((long long)zNMask & (kPrimeZ2)),
                x1, (double)(yNMask | 1) + y1, (double)(zNMask | 1) + z1);
        }
    }

    if (!skip9) {
        const double a9 = xAFlipMask1 + zAFlipMask1 + a1;
        if (a9 > 0) {
            value += (a9 * a9) * (a9 * a9) * grad3(seed2,
                xrbp + ((long long)xNMask & (kPrimeX2)),
                yrbp + kPrimeY,
                zrbp + ((long long)zNMask & (kPrimeZ2)),
                (double)(xNMask | 1) + x1, y1, (double)(zNMask | 1) + z1);
        }
    }

    if (!skipD) {
        const double aD = xAFlipMask1 + yAFlipMask1 + a1;
        if (aD > 0) {
            value += (aD * aD) * (aD * aD) * grad3(seed2,
                xrbp + ((long long)xNMask & (kPrimeX2)),
                yrbp + ((long long)yNMask & (kPrimeY2)),
                zrbp + kPrimeZ,
                (double)(xNMask | 1) + x1, (double)(yNMask | 1) + y1, z1);
        }
    }

    return static_cast<T>(value);
}

template <typename T>
void OpenSimplex2S<T>::fill(T* data, std::size_t width, std::size_t height,
                            T scale_x, T scale_y) const
{
    for (std::size_t row = 0; row < height; ++row) {
        for (std::size_t col = 0; col < width; ++col) {
            const T v = noise(static_cast<T>(col) * scale_x,
                              static_cast<T>(row) * scale_y);
            data[row * width + col] = v * T(0.5) + T(0.5);
        }
    }
}

template <typename T>
void OpenSimplex2S<T>::fill(T* data, std::size_t width, std::size_t height, std::size_t depth,
                            T scale_x, T scale_y, T scale_z) const
{
    for (std::size_t layer = 0; layer < depth; ++layer) {
        for (std::size_t row = 0; row < height; ++row) {
            for (std::size_t col = 0; col < width; ++col) {
                const T v = noise(static_cast<T>(col) * scale_x,
                                  static_cast<T>(row) * scale_y,
                                  static_cast<T>(layer) * scale_z);
                data[(layer * height + row) * width + col] = v * T(0.5) + T(0.5);
            }
        }
    }
}

#endif
