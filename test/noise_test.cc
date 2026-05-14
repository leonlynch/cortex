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

static void test_range_3d_yup()
{
	OpenSimplex2S<double> n(12345);
	double vmin = 1.0, vmax = -1.0;
	for (int z = 0; z < 16; ++z) {
		for (int y = 0; y < 16; ++y) {
			for (int x = 0; x < 16; ++x) {
				double v = n.noiseYUp(x * 0.1, y * 0.1, z * 0.1);
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
	std::printf("3D Y-up range: [%.4f, %.4f]\n", vmin, vmax);
}

static void test_reproducibility_yup()
{
	OpenSimplex2S<double> a(99999), b(99999);
	for (int i = 0; i < 20; ++i) {
		double va = a.noiseYUp(i * 0.37, i * 0.53, i * 0.19);
		double vb = b.noiseYUp(i * 0.37, i * 0.53, i * 0.19);
		assert(va == vb);
	}
	std::printf("Y-up reproducibility: OK\n");
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

static void test_range_4d()
{
	OpenSimplex2S<double> n(12345);
	double vmin = 1.0, vmax = -1.0;
	for (int w = 0; w < 4; ++w) {
		for (int z = 0; z < 8; ++z) {
			for (int y = 0; y < 8; ++y) {
				for (int x = 0; x < 8; ++x) {
					double v = n.noise(x * 0.1, y * 0.1, z * 0.1, w * 0.1);
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
	}
	std::printf("4D range: [%.4f, %.4f]\n", vmin, vmax);
}

static void test_range_4d_yup()
{
	OpenSimplex2S<double> n(12345);
	double vmin = 1.0, vmax = -1.0;
	for (int w = 0; w < 4; ++w) {
		for (int z = 0; z < 8; ++z) {
			for (int y = 0; y < 8; ++y) {
				for (int x = 0; x < 8; ++x) {
					double v = n.noiseYUp(x * 0.1, y * 0.1, z * 0.1, w * 0.1);
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
	}
	std::printf("4D Y-up range: [%.4f, %.4f]\n", vmin, vmax);
}

static void test_reproducibility_4d()
{
	OpenSimplex2S<double> a(99999), b(99999);
	for (int i = 0; i < 20; ++i) {
		double va = a.noise(i * 0.37, i * 0.53, i * 0.19, i * 0.11);
		double vb = b.noise(i * 0.37, i * 0.53, i * 0.19, i * 0.11);
		assert(va == vb);
	}
	std::printf("4D reproducibility: OK\n");
}

static void test_reproducibility_4d_yup()
{
	OpenSimplex2S<double> a(99999), b(99999);
	for (int i = 0; i < 20; ++i) {
		double va = a.noiseYUp(i * 0.37, i * 0.53, i * 0.19, i * 0.11);
		double vb = b.noiseYUp(i * 0.37, i * 0.53, i * 0.19, i * 0.11);
		assert(va == vb);
	}
	std::printf("4D Y-up reproducibility: OK\n");
}

static void test_normals_gradient()
{
	// Verify the analytical gradient via finite differences.
	// fillNormals stores normalize(-ddx, -ddy, 1), so nx/nz = -ddx, ny/nz = -ddy.
	static const int W = 32, H = 32;
	static const double scale = 0.1;
	static const double eps   = 1e-6;
	static const double tol   = 1e-5;
	OpenSimplex2S<double> n(12345);
	std::vector<double> buf(3 * W * H);
	n.fillNormals(buf.data(), W, H, scale, scale);
	for (int row = 1; row < H - 1; ++row) {
		for (int col = 1; col < W - 1; ++col) {
			const double x = col * scale, y = row * scale;
			const double* nm = &buf[3 * (row * W + col)];
			const double nx = nm[0], nz = nm[2];
			const double ny = nm[1];
			const double ddx = -nx / nz;
			const double ddy = -ny / nz;
			const double fd_ddx = (n.noise(x + eps, y) - n.noise(x - eps, y)) / (2.0 * eps);
			const double fd_ddy = (n.noise(x, y + eps) - n.noise(x, y - eps)) / (2.0 * eps);
			assert(std::fabs(ddx - fd_ddx) < tol);
			assert(std::fabs(ddy - fd_ddy) < tol);
		}
	}
	std::printf("normals gradient: OK\n");
}

static void test_normals_unit_length()
{
	static const int W = 32, H = 32;
	OpenSimplex2S<double> n(99999);
	std::vector<double> buf(3 * W * H);
	n.fillNormals(buf.data(), W, H, 0.1, 0.1);
	for (int i = 0; i < W * H; ++i) {
		const double nx = buf[3*i], ny = buf[3*i+1], nz = buf[3*i+2];
		const double len = std::sqrt(nx*nx + ny*ny + nz*nz);
		assert(std::fabs(len - 1.0) < 1e-12);
		assert(nz > 0.0);
	}
	std::printf("normals unit length: OK\n");
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
	static const double kExpected3DYUp[20] = {
		 0,
		 0.019923649947901214,
		-0.49109337199328623,
		-0.29284827032113536,
		-0.036046422046814179,
		 0.11104747016588055,
		-0.53491555554976744,
		-0.14439461080417848,
		 0.28778543530876327,
		 0.43701312332518633,
		-0.51047551147881143,
		-0.27791226315596451,
		-0.21826134722635435,
		 0.30769123621093331,
		-0.65369105858210652,
		 0.46707575425069925,
		-0.084076568704406107,
		 0.040012606472853506,
		-0.23969075804240284,
		 0.091583630573677854,
	};
	static const double kExpected4D[20] = {
		 1.4670823273711559e-56,
		 0.27330863297505309,
		-0.1247888424870798,
		-0.068170140922254138,
		-0.22877162010870356,
		-0.16977632199324261,
		 0.35569084629761261,
		 0.0079685035618279904,
		-0.71606323383249804,
		-0.22624759994732929,
		 0.013490129153863035,
		 0.39839632680988324,
		-0.020335440422609836,
		 0.41517895653208048,
		-0.27376347252267358,
		-0.12402542513643491,
		-0.10486329312373374,
		-0.43334789907510096,
		 0.053204955594503799,
		-0.19000227245904527,
	};
	static const double kExpected4DYUp[20] = {
		 1.4670823273711559e-56,
		 0.2663686468163794,
		 0.3177174966255934,
		 0.60068400466844818,
		 0.16813566259378601,
		 0.17934514851579142,
		 0.01885747941952623,
		-0.075464615015133296,
		 0.10971185906345537,
		 0.12258924292931596,
		-0.081817631704340171,
		-0.11609734579630533,
		 0.2306211812600642,
		 0.054666441888276525,
		 0.43276925109358655,
		-0.5181076106015613,
		 0.22916695109089857,
		 0.36960190780554347,
		 0.12659867239448339,
		-0.0024900352290644881,
	};

	OpenSimplex2S<double> n(12345);
	for (int i = 0; i < 20; ++i) {
		assert(n.noise(i * 0.37, i * 0.53) == kExpected2D[i]);
		assert(n.noise(i * 0.37, i * 0.53, i * 0.19) == kExpected3D[i]);
		assert(n.noiseYUp(i * 0.37, i * 0.53, i * 0.19) == kExpected3DYUp[i]);
		assert(n.noise(i * 0.37, i * 0.53, i * 0.19, i * 0.11) == kExpected4D[i]);
		assert(n.noiseYUp(i * 0.37, i * 0.53, i * 0.19, i * 0.11) == kExpected4DYUp[i]);
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
	test_range_3d_yup();
	test_reproducibility();
	test_reproducibility_yup();
	test_float_instantiation();
	test_continuity_2d();
	test_fill_range();
	test_range_4d();
	test_range_4d_yup();
	test_reproducibility_4d();
	test_reproducibility_4d_yup();
	test_normals_gradient();
	test_normals_unit_length();
	test_golden_values();
	print_ascii_2d();
	return 0;
}
