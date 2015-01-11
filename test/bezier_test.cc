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

std::ostream& operator<< (std::ostream& os, const glm::vec2& v)
{
	os << "(";
	for (auto i = 0; i < v.length(); ++i) {
		if (i)
			os << ", ";
		os << v[i];
	}
	os << ")";

	return os;
}

std::ostream& operator<< (std::ostream& os, const glm::vec3& v)
{
	os << "(";
	for (auto i = 0; i < v.length(); ++i) {
		if (i)
			os << ", ";
		os << v[i];
	}
	os << ")";

	return os;
}

template <typename T, unsigned int n>
void print_bezier_eval(const BezierCurve<T,n>& bc, unsigned int count)
{
	for (auto i = 0; i < count + 1; ++i) {
		double t = i / static_cast<double>(count);

		glm::vec2 position = bc.position(t);
		std::cout << "p: " << position << "\n";
	}
}

template <typename T, unsigned int n, unsigned int m>
void print_bezier_eval(const BezierSurface<T,n,m>& bs, unsigned int u_count, unsigned int v_count)
{
	for (auto i = 0; i < u_count + 1; ++i) {
		for (auto j = 0; j < v_count + 1; ++j) {
			double u = i / static_cast<double>(u_count);
			double v = j / static_cast<double>(v_count);

			glm::vec3 position = bs.position(u, v);
			std::cout << "p: " << position << "\n";
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

	std::cout << "k: " << bc_vec2 << "\n";
	print_bezier_eval(bc_vec2, 6);
	std::cout << "\n";

	glm::vec3 bs_k[4][4] = {
		{ { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.25f, 0.0f }, { 0.0f, 0.75f, 0.0f }, { 0.0f, 1.0f, 0.0f }, },
		{ { 0.25f, 0.0f, 0.0f }, { 0.25f, 0.25f, 0.25f }, { 0.25f, 0.75f, 0.25f }, { 0.25f, 1.0f, 0.0f }, },
		{ { 0.75f, 0.0f, 0.0f }, { 0.75f, 0.25f, 0.25f }, { 0.75f, 0.75f, 0.25f }, { 0.75f, 1.0f, 0.0f }, },
		{ { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.25f, 0.0f }, { 1.0f, 0.75f, 0.0f }, { 1.0f, 1.0f, 0.0f }, },
	};
	BezierSurface<glm::vec3, 3, 3> bs_vec3(bs_k);

	std::cout << "k: \n" << bs_vec3 << "\n";
	print_bezier_eval(bs_vec3, 6, 6);
	std::cout << "\n";
}
