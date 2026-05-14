/**
 * @file noise.h
 *
 * Copyright (c) 2026 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#ifndef CORTEX_NOISE_H
#define CORTEX_NOISE_H

#include <cstddef>

/**
 * @brief OpenSimplex2S (Smooth) gradient noise.
 *
 * C3-continuous gradient noise based on Kurt Spencer's OpenSimplex2S
 * algorithm. The 2D variant uses a triangular lattice; the 3D variants use a
 * BCC lattice.
 *
 * Internal arithmetic is always double regardless of T. The final result is
 * cast to T. Use T = float where half the memory footprint matters more than
 * precision. Use T = double (the default) otherwise.
 *
 * Input coordinates are taken as-is. Scale them to control the feature size
 * of the noise.
 */
template <typename T = double>
struct OpenSimplex2S
{
	/**
	 * @brief Construct with the given seed.
	 *
	 * The same seed always produces identical output across all overloads and
	 * all platforms.
	 */
	explicit OpenSimplex2S(long long seed = 0);

	/**
	 * @brief Compute 2D noise at (x, y).
	 *
	 * The distribution is roughly uniform. Values near the extremes are rare.
	 *
	 * @return Noise value in the range [-1, 1] at (x, y).
	 */
	T noise(T x, T y) const;

	/**
	 * @brief Compute 3D noise at (x, y, z) with better visual isotropy in the
	 *        XY plane.
	 *
	 * The distribution is roughly uniform. Values near the extremes are rare.
	 * Z is the "special" (least-isotropic) axis. Use when Z is the vertical
	 * world axis, or as the time dimension in an animated texture.
	 *
	 * @return Noise value in the range [-1, 1] at (x, y, z).
	 */
	T noise(T x, T y, T z) const;

	/**
	 * @brief Compute 3D noise at (x, y, z) with better visual isotropy in the
	 *        XZ plane.
	 *
	 * The distribution is roughly uniform. Values near the extremes are rare.
	 * Y is the "special" (least-isotropic) axis. Use when Y is the vertical
	 * world axis (a common convention).
	 *
	 * @return Noise value in the range [-1, 1] at (x, y, z).
	 */
	T noiseYUp(T x, T y, T z) const;

	/**
	 * @brief Compute 4D noise at (x, y, z, w) with XY and ZW as orthogonal
	 *        isotropic planes.
	 *
	 * The distribution is roughly uniform. Values near the extremes are rare.
	 * The canonical use is noise(x, y, sin(t), cos(t)) such that (z, w) traces
	 * a circle seamlessly looping over t. Use for animated textures.
	 *
	 * @return Noise value in the range [-1, 1] at (x, y, z, w).
	 */
	T noise(T x, T y, T z, T w) const;

	/**
	 * @brief Compute 4D noise at (x, y, z, w) with XZ as the isotropic
	 *        horizontal plane (Y-up) and W as an independent time dimension.
	 *
	 * The distribution is roughly uniform. Values near the extremes are rare.
	 * The orientation matches noiseYUp(x, y, z) with Y as the "special"
	 * (least-isotropic) axis. Use when Y is the vertical world axis for
	 * time-animated terrain.
	 *
	 * @return Noise value in the range [-1, 1] at (x, y, z, w).
	 */
	T noiseYUp(T x, T y, T z, T w) const;

	/**
	 * @brief Fill a row-major width×height buffer with 2D noise remapped to
	 *        [0, 1].
	 *
	 * Pixel (col, row) is sampled at (col * scale_x, row * scale_y) and stored
	 * at data[row * width + col]. The @p data buffer must point to at least
	 * width * height elements. The output range [0, 1] makes the buffer
	 * suitable for glTextureSubImage2D using internal format GL_R32F / GL_RED.
	 *
	 * Scaling parameters control feature frequency: larger values produce
	 * higher-frequency (smaller-scale) features.
	 *
	 * @param data Output buffer
	 * @param width Buffer width (x) in columns
	 * @param height Buffer height (y) in rows
	 * @param scale_x Column (x) sampling frequency
	 * @param scale_y Row (y) sampling frequency
	 */
	void fill(
		T* data,
		std::size_t width,
		std::size_t height,
		T scale_x = T(1),
		T scale_y = T(1)
	) const;

	/**
	 * @brief Fill a row-major width×height×depth buffer with 3D noise
	 *        remapped to [0, 1].
	 *
	 * Voxel (col, row, layer) is sampled at (col * scale_x, row * scale_y,
	 * layer * scale_z) and stored at data[(layer * height + row) * width +
	 * col]. The @p data buffer must point to at least width * height * depth
	 * elements. The output range [0, 1] makes the buffer suitable for
	 * glTextureSubImage3D using internal format GL_R32F / GL_RED.
	 *
	 * Scaling parameters control feature frequency: larger values produce
	 * higher-frequency (smaller-scale) features.
	 *
	 * @param data Output buffer
	 * @param width Buffer width (x) in columns
	 * @param height Buffer height (y) in rows
	 * @param depth Buffer depth (z) in layers
	 * @param scale_x Column (x) sampling frequency
	 * @param scale_y Row (y) sampling frequency
	 * @param scale_z Layer (z) sampling frequency
	 */
	void fill(
		T* data,
		std::size_t width,
		std::size_t height,
		std::size_t depth,
		T scale_x = T(1),
		T scale_y = T(1),
		T scale_z = T(1)
	) const;

	/**
	 * @brief Fill a row-major width×height buffer with unit-length surface
	 *        normals derived from the 2D noise gradient.
	 *
	 * Each normal is stored as three consecutive elements (nx, ny, nz) at
	 * data[3*(row*width + col)]. The @p data buffer must hold at least
	 * 3*width*height elements. The output is suitable for glTextureSubImage2D
	 * using internal format GL_RGB32F / GL_RGB.
	 *
	 * Each normal is computed as normalize(−ddx, −ddy, 1), where (ddx, ddy)
	 * is the gradient of the noise. This ensures that nz, the component
	 * perpendicular to the noise plane, is always positive.
	 *
	 * Scaling parameters control feature frequency: larger values produce
	 * higher-frequency (smaller-scale) features.
	 *
	 * @param data Output buffer
	 * @param width Buffer width (x) in columns
	 * @param height Buffer height (y) in rows
	 * @param scale_x Column (x) sampling frequency
	 * @param scale_y Row (y) sampling frequency
	 */
	void fillNormals(
		T* data,
		std::size_t width,
		std::size_t height,
		T scale_x = T(1),
		T scale_y = T(1)
	) const;

private:
	long long seed_;
};

#include "noise.tcc"

#endif
