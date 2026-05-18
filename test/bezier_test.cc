/**
 * @file bezier_test.cc
 *
 * Copyright 2013, 2026 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include "bezier.h"

#include <cstdio>

#include <iostream>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL // Because string_cast.hpp includes dual_quaternion.hpp
#include <glm/gtx/string_cast.hpp>

template <typename T>
struct vertex_t
{
	T position;
	T normal;
};

template <typename T>
struct vertex_with_texcoord_t
{
	T position;
	T normal;
	glm::vec<T::length() - 1, typename T::value_type> texcoord;
};

template <typename T>
struct vertex_with_scalar_texcoord_t
{
	T position;
	T normal;
	typename T::value_type texcoord;
};

template<glm::length_t N, typename T, glm::qualifier Q>
std::ostream& operator<< (std::ostream& os, const glm::vec<N, T, Q>& v)
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

template <typename VertexType, typename T, std::size_t n>
void print_bezier_eval(const BezierCurve<T,n>& bc, std::size_t t_count)
{
	std::vector<VertexType> vertices;
	std::vector<unsigned int> indices;
	bc.tessellate(t_count, vertices, indices);

	for (auto&& vertex : vertices) {
		std::cout << "p: " << vertex.position << "; n: " << vertex.normal;
		if constexpr (detail::has_texcoord<VertexType>::value)
			std::cout << "; t: " << vertex.texcoord;
		std::cout << "\n";
	}

	std::cout << "indices: ";
	for (auto&& index : indices)
		std::cout << index << " ";
	std::cout << "\n";
}

template <typename VertexType, typename T, std::size_t n, std::size_t m>
void print_bezier_eval(const BezierSurface<T,n,m>& bs, std::size_t u_count, std::size_t v_count)
{
	std::vector<VertexType> vertices;
	std::vector<unsigned int> indices;
	bs.tessellate(u_count, v_count, vertices, indices);

	for (auto&& vertex : vertices) {
		std::cout << "p: " << vertex.position << "; n: " << vertex.normal;
		if constexpr (detail::has_texcoord<VertexType>::value)
			std::cout << "; uv: " << vertex.texcoord;
		std::cout << "\n";
	}

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

	printf("Test BezierCurve with glm::vec2 position and normal...\n");
	std::cout << "k: " << bc_vec2 << "\n";
	print_bezier_eval<vertex_t<glm::vec2>>(bc_vec2, 6);
	std::cout << "\n";

	printf("Test BezierCurve with glm::vec1 texcoord...\n");
	std::cout << "k: " << bc_vec2 << "\n";
	print_bezier_eval<vertex_with_texcoord_t<glm::vec2>>(bc_vec2, 6);
	std::cout << "\n";

	printf("Test BezierCurve with float texcoord...\n");
	std::cout << "k: " << bc_vec2 << "\n";
	print_bezier_eval<vertex_with_scalar_texcoord_t<glm::vec2>>(bc_vec2, 6);
	std::cout << "\n";

	glm::vec3 bs_k[4][4] = {
		{ { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.25f, 0.0f }, { 0.0f, 0.75f, 0.0f }, { 0.0f, 1.0f, 0.0f }, },
		{ { 0.25f, 0.0f, 0.0f }, { 0.25f, 0.25f, 0.25f }, { 0.25f, 0.75f, 0.25f }, { 0.25f, 1.0f, 0.0f }, },
		{ { 0.75f, 0.0f, 0.0f }, { 0.75f, 0.25f, 0.25f }, { 0.75f, 0.75f, 0.25f }, { 0.75f, 1.0f, 0.0f }, },
		{ { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.25f, 0.0f }, { 1.0f, 0.75f, 0.0f }, { 1.0f, 1.0f, 0.0f }, },
	};
	BezierSurface<glm::vec3, 3, 3> bs_vec3(bs_k);

	printf("Test BezierSurface with glm::vec3 position and normal...\n");
	std::cout << "k: \n" << bs_vec3 << "\n";
	using Vertex = vertex_t<glm::vec3>;
	print_bezier_eval<Vertex>(bs_vec3, 6, 6);
	std::cout << "\n";

	printf("Test BezierSurface with glm::vec2 texcoord...\n");
	std::cout << "k: \n" << bs_vec3 << "\n";
	using VertexWithTexcoord = vertex_with_texcoord_t<glm::vec3>;
	print_bezier_eval<VertexWithTexcoord>(bs_vec3, 6, 6);
	std::cout << "\n";
}
