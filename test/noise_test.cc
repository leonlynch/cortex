/**
 * @file noise_test.cc
 *
 * Copyright (c) 2026 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include "noise.h"

#include <cstdio>
#include <cmath>
#include <cassert>
#include <string>
#include <vector>

static void test_range_2d()
{
	OpenSimplex2S<double> n(12345);
	double vmin = 1.0, vmax = -1.0;
	for (int y = 0; y < 64; ++y) {
		for (int x = 0; x < 64; ++x) {
			double v = n.noise(x * 0.1, y * 0.1);
			if (v < vmin) {
				vmin = v;
			}
			if (v > vmax) {
				vmax = v;
			}
			assert(v >= -1.0 && v <= 1.0);
		}
	}
	std::printf("2D range: [%.4f, %.4f]\n", vmin, vmax);
}

static void test_range_3d()
{
	OpenSimplex2S<double> n(12345);
	double vmin = 1.0, vmax = -1.0;
	for (int z = 0; z < 16; ++z) {
		for (int y = 0; y < 16; ++y) {
			for (int x = 0; x < 16; ++x) {
				double v = n.noise(x * 0.1, y * 0.1, z * 0.1);
				if (v < vmin) {
					vmin = v;
				}
				if (v > vmax) {
					vmax = v;
				}
				assert(v >= -1.0 && v <= 1.0);
			}
		}
	}
	std::printf("3D range: [%.4f, %.4f]\n", vmin, vmax);
}

static void test_reproducibility()
{
	OpenSimplex2S<double> a(99999), b(99999);
	for (int i = 0; i < 20; ++i) {
		double va = a.noise(i * 0.37, i * 0.53);
		double vb = b.noise(i * 0.37, i * 0.53);
		assert(va == vb);
	}
	std::printf("reproducibility: OK\n");
}

static void test_float_instantiation()
{
	OpenSimplex2S<float> n(42);
	for (int y = 0; y < 16; ++y) {
		for (int x = 0; x < 16; ++x) {
			float v = n.noise(x * 0.2f, y * 0.2f);
			assert(v >= -1.0f && v <= 1.0f);
		}
	}
	std::printf("float instantiation: OK\n");
}

static void test_continuity_2d()
{
	// Measure maximum first and second finite differences along a 1D slice.
	OpenSimplex2S<double> n(7777);
	static const int N = 10000;
	static const double H = 0.001;
	double max_d1 = 0.0, max_d2 = 0.0;
	double prev = n.noise(0.0, 0.0);
	double prev_d1 = n.noise(H, 0.0) - prev;
	for (int i = 1; i < N - 1; ++i) {
		double curr = n.noise(i * H, 0.0);
		double next = n.noise((i + 1) * H, 0.0);
		double d1 = next - curr;
		double d2 = (d1 - prev_d1) / H;
		double ad1 = std::fabs(d1 / H);
		double ad2 = std::fabs(d2 / H);
		if (ad1 > max_d1) {
			max_d1 = ad1;
		}
		if (ad2 > max_d2) {
			max_d2 = ad2;
		}
		prev = curr;
		prev_d1 = d1;
	}
	std::printf("continuity: max |f'| = %.4f, max |f''| = %.4f\n", max_d1, max_d2);
}

static void test_fill_range()
{
	OpenSimplex2S<double> n(555);
	static const std::size_t W = 32, H = 32;
	std::vector<double> buf(W * H);
	n.fill(buf.data(), W, H, 0.05, 0.05);
	for (double v : buf) {
		assert(v >= 0.0 && v <= 1.0);
	}
	std::printf("fill() range: OK\n");
}

static void test_golden_values()
{
	// Reference values based on the initial implementation to detect
	// future regression
	static const double kExpected2D[20] = {
		-2.2169018254728484e-61,
		-0.4972228173927758,
		-0.21844783030419829,
		 0.86490787945732917,
		-0.79758600702043592,
		 0.01674427549009988,
		 0.45901477497763521,
		 0.22240491151887351,
		-0.091452380358384128,
		-0.9716798645061262,
		-0.62990515049606588,
		-0.31802963633363074,
		 0.2543478619258418,
		-0.30848878023603349,
		-0.56543413104073592,
		 0.10829413597859926,
		 0.62328527671559819,
		-0.029574116090302881,
		-0.82824305153680022,
		 0.42856690032109013,
	};
	static const double kExpected3D[20] = {
		 0,
		 0.40265644606844642,
		-0.33120017118808015,
		-0.2775791767731971,
		 0.24343411176261218,
		 0.040743336133320734,
		-0.35087702218973371,
		 0.5575115492972712,
		-0.57136544887819996,
		-0.31480069488011636,
		 0.27904977886943588,
		-0.44154552299967237,
		-0.23869272340027275,
		-0.091340310917887801,
		-0.38180530983151817,
		-0.30002881212755539,
		 0.49535409219798593,
		-0.60852847426391576,
		 0.17602466641043996,
		-0.55438765368935239,
	};

	OpenSimplex2S<double> n(12345);
	for (int i = 0; i < 20; ++i) {
		assert(n.noise(i * 0.37, i * 0.53) == kExpected2D[i]);
		assert(n.noise(i * 0.37, i * 0.53, i * 0.19) == kExpected3D[i]);
	}
	std::printf("golden values: OK\n");
}

static void print_ascii_2d()
{
	static const char kRamp[] = " .:-=+*#%@";
	static const int kRampLen = 10;
	OpenSimplex2S<double> n(12345);
	static const int W = 64, H = 32;
	std::printf("\n2D noise (64x32):\n");
	for (int row = 0; row < H; ++row) {
		for (int col = 0; col < W; ++col) {
			double v = n.noise(col * 0.12, row * 0.12);
			int idx = static_cast<int>((v * 0.5 + 0.5) * (kRampLen - 1) + 0.5);
			if (idx < 0) {
				idx = 0;
			}
			if (idx >= kRampLen) {
				idx = kRampLen - 1;
			}
			std::putchar(kRamp[idx]);
		}
		std::putchar('\n');
	}
}

int main(void)
{
	test_range_2d();
	test_range_3d();
	test_reproducibility();
	test_float_instantiation();
	test_continuity_2d();
	test_fill_range();
	test_golden_values();
	print_ascii_2d();
	return 0;
}
