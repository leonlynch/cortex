/**
 * @file bezier_test.cc
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include "bezier.h"

#include <cstdio>

#include <string>
#include <iostream>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

template <typename T, unsigned int n>
void print_bezier_eval(const BezierCurve<T,n>& bc, unsigned int count)
{
	for (auto i = 0; i < count + 1; ++i) {
		double t = i / static_cast<double>(count);

		glm::vec2 p = bc.eval(t);
		std::printf("%s\n", glm::to_string(p).c_str());
	}
}

template <typename T, unsigned int n, unsigned int m>
void print_bezier_eval(const BezierSurface<T,n,m>& bs, unsigned int u_count, unsigned int v_count)
{
	for (auto i = 0; i < u_count + 1; ++i) {
		for (auto j = 0; j < v_count + 1; ++j) {
			double u = i / static_cast<double>(u_count);
			double v = j / static_cast<double>(v_count);

			glm::vec3 p = bs.eval(u, v);
			std::printf("%s\n", glm::to_string(p).c_str());
		}
	}
}

int main(void)
{
	glm::vec2 bc_k[4] = {
		{ 0.0f, 0.0f },
		{ 0.25f, 1.0f },
		{ 0.75f, 1.0f },
		{ 1.0f, 0.0f },
	};
	BezierCurve<glm::vec2, 3> bc_vec2(bc_k);

	std::cout << bc_vec2 << "\n";
	print_bezier_eval(bc_vec2, 6);

	glm::vec3 bs_k[4][4] = {
		{ { 0.0f, 0.0f, 0.0f }, { 0.25f, 0.0f, 0.0f }, { 0.75f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, },
		{ { 0.0f, 0.25f, 0.0f }, { 0.25f, 0.25f, 1.0f }, { 0.75f, 0.25f, 1.0f }, { 1.0f, 0.25f, 0.0f }, },
		{ { 0.0f, 0.75f, 0.0f }, { 0.25f, 0.75f, 1.0f }, { 0.75f, 0.75f, 1.0f }, { 1.0f, 0.75f, 0.0f }, },
		{ { 0.0f, 1.0f, 0.0f }, { 0.25f, 1.0f, 0.0f }, { 0.75f, 1.0f, 0.0f }, { 1.0f, 1.0f, 0.0f }, },
	};
	BezierSurface<glm::vec3, 3, 3> bs_vec3(bs_k);

	std::cout << bs_vec3 << "\n";
	print_bezier_eval(bs_vec3, 6, 6);
}
