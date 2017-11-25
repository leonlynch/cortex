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

template <typename T, std::size_t n>
void print_bezier_eval(const BezierCurve<T,n>& bc, std::size_t t_count)
{
	std::vector<typename BezierCurve<T,n>::Vertex> vertices;
	std::vector<unsigned int> indices;
	bc.tesselate(t_count, vertices, indices);

	for (auto&& vertex : vertices)
		std::cout << "p: " << vertex.position << "; n: " << vertex.normal << "\n";

	std::cout << "indices: ";
	for (auto&& index : indices)
		std::cout << index << " ";
	std::cout << "\n";
}

template <typename T, std::size_t n, std::size_t m>
void print_bezier_eval(const BezierSurface<T,n,m>& bs, std::size_t u_count, std::size_t v_count)
{
	std::vector<typename BezierSurface<T,n,m>::Vertex> vertices;
	std::vector<unsigned int> indices;
	bs.tesselate(u_count, v_count, vertices, indices);

	for (auto&& vertex : vertices)
		std::cout << "p: " << vertex.position << "; n: " << vertex.normal << "\n";

	std::cout << "indices: ";
	for (auto&& index : indices)
		std::cout << index << " ";
	std::cout << "\n";
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
