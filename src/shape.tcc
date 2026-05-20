/**
 * @file shape.tcc
 *
 * Copyright (c) 2020, 2026 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include "shape.h"

#ifndef CORTEX_SHAPE_TCC
#define CORTEX_SHAPE_TCC

#include "vertex_traits.h"

template <typename VertexType, typename IndexType>
void Cube::tessellate(std::vector<VertexType>& vertices, std::vector<IndexType>& indices) const
{
	static_assert(std::is_default_constructible<VertexType>::value, "VertexType must be default-constructible");
	static_assert(std::is_integral<IndexType>::value, "IndexType must be an integer type");

	vertices.reserve(vertices.size() + 24);
	indices.reserve(indices.size() + 36);

	std::size_t offset = vertices.size();

	struct raw_vertex_t {
		float position[3];
		float normal[3];
		float texcoord[2];
	};
	static const raw_vertex_t raw_vertices[] = {
		// Front (+Z): u=(x+1)/2, v=(y+1)/2
		{ {  1.0f,  1.0f,  1.0f }, {  0.0f,  0.0f,  1.0f }, { 1.0f, 1.0f } },
		{ { -1.0f,  1.0f,  1.0f }, {  0.0f,  0.0f,  1.0f }, { 0.0f, 1.0f } },
		{ { -1.0f, -1.0f,  1.0f }, {  0.0f,  0.0f,  1.0f }, { 0.0f, 0.0f } },
		{ {  1.0f, -1.0f,  1.0f }, {  0.0f,  0.0f,  1.0f }, { 1.0f, 0.0f } },
		// Back (-Z): u=(1-x)/2, v=(y+1)/2
		{ { -1.0f,  1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 1.0f } },
		{ {  1.0f,  1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f }, { 0.0f, 1.0f } },
		{ {  1.0f, -1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f }, { 0.0f, 0.0f } },
		{ { -1.0f, -1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 0.0f } },
		// Right (+X): u=(1-z)/2, v=(y+1)/2
		{ {  1.0f,  1.0f, -1.0f }, {  1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f } },
		{ {  1.0f,  1.0f,  1.0f }, {  1.0f,  0.0f,  0.0f }, { 0.0f, 1.0f } },
		{ {  1.0f, -1.0f,  1.0f }, {  1.0f,  0.0f,  0.0f }, { 0.0f, 0.0f } },
		{ {  1.0f, -1.0f, -1.0f }, {  1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f } },
		// Left (-X): u=(z+1)/2, v=(y+1)/2
		{ { -1.0f,  1.0f,  1.0f }, { -1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f } },
		{ { -1.0f,  1.0f, -1.0f }, { -1.0f,  0.0f,  0.0f }, { 0.0f, 1.0f } },
		{ { -1.0f, -1.0f, -1.0f }, { -1.0f,  0.0f,  0.0f }, { 0.0f, 0.0f } },
		{ { -1.0f, -1.0f,  1.0f }, { -1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f } },
		// Top (+Y): u=(x+1)/2, v=(1-z)/2
		{ {  1.0f,  1.0f, -1.0f }, {  0.0f,  1.0f,  0.0f }, { 1.0f, 1.0f } },
		{ { -1.0f,  1.0f, -1.0f }, {  0.0f,  1.0f,  0.0f }, { 0.0f, 1.0f } },
		{ { -1.0f,  1.0f,  1.0f }, {  0.0f,  1.0f,  0.0f }, { 0.0f, 0.0f } },
		{ {  1.0f,  1.0f,  1.0f }, {  0.0f,  1.0f,  0.0f }, { 1.0f, 0.0f } },
		// Bottom (-Y): u=(x+1)/2, v=(z+1)/2
		{ {  1.0f, -1.0f,  1.0f }, {  0.0f, -1.0f,  0.0f }, { 1.0f, 1.0f } },
		{ { -1.0f, -1.0f,  1.0f }, {  0.0f, -1.0f,  0.0f }, { 0.0f, 1.0f } },
		{ { -1.0f, -1.0f, -1.0f }, {  0.0f, -1.0f,  0.0f }, { 0.0f, 0.0f } },
		{ {  1.0f, -1.0f, -1.0f }, {  0.0f, -1.0f,  0.0f }, { 1.0f, 0.0f } },
	};
	for (const auto& rv : raw_vertices) {
		VertexType v{};
		v.position = { rv.position[0], rv.position[1], rv.position[2] };
		if constexpr (detail::has_normal<VertexType>::value) {
			v.normal = { rv.normal[0], rv.normal[1], rv.normal[2] };
		}
		if constexpr (detail::has_texcoord<VertexType>::value) {
			using S = typename decltype(v.texcoord)::value_type;
			v.texcoord = { static_cast<S>(rv.texcoord[0]), static_cast<S>(rv.texcoord[1]) };
		}
		vertices.push_back(v);
	}

	static const unsigned int raw_indices[] = {
		// Front (+Z)
		0, 1, 3,
		2, 3, 1,
		// Back (-Z)
		4, 5, 7,
		6, 7, 5,
		// Right (+X)
		8, 9, 11,
		10, 11, 9,
		// Left (-X)
		12, 13, 15,
		14, 15, 13,
		// Top (+Y)
		16, 17, 19,
		18, 19, 17,
		// Bottom (-Y)
		20, 21, 23,
		22, 23, 21,
	};
	for (auto idx : raw_indices) {
		indices.push_back(static_cast<IndexType>(offset + idx));
	}
}

template <typename VertexType, typename IndexType>
void Octahedron::tessellate(std::vector<VertexType>& vertices, std::vector<IndexType>& indices) const
{
	static_assert(std::is_default_constructible<VertexType>::value, "VertexType must be default-constructible");
	static_assert(std::is_integral<IndexType>::value, "IndexType must be an integer type");

	vertices.reserve(vertices.size() + 24);
	indices.reserve(indices.size() + 24);

	std::size_t offset = vertices.size();

	struct raw_vertex_with_normal_t {
		float position[3];
		float normal[3];
	};
	static const raw_vertex_with_normal_t raw_vertices[] = {
		{ {  1.0f,  0.0f,  0.0f }, {  1.0f,  1.0f,  1.0f } },
		{ {  0.0f,  1.0f,  0.0f }, {  1.0f,  1.0f,  1.0f } },
		{ {  0.0f,  0.0f,  1.0f }, {  1.0f,  1.0f,  1.0f } },

		{ {  1.0f,  0.0f,  0.0f }, {  1.0f,  1.0f, -1.0f } },
		{ {  0.0f,  0.0f, -1.0f }, {  1.0f,  1.0f, -1.0f } },
		{ {  0.0f,  1.0f,  0.0f }, {  1.0f,  1.0f, -1.0f } },

		{ {  1.0f,  0.0f,  0.0f }, {  1.0f, -1.0f, -1.0f } },
		{ {  0.0f, -1.0f,  0.0f }, {  1.0f, -1.0f, -1.0f } },
		{ {  0.0f,  0.0f, -1.0f }, {  1.0f, -1.0f, -1.0f } },

		{ {  1.0f,  0.0f,  0.0f }, {  1.0f, -1.0f,  1.0f } },
		{ {  0.0f,  0.0f,  1.0f }, {  1.0f, -1.0f,  1.0f } },
		{ {  0.0f, -1.0f,  0.0f }, {  1.0f, -1.0f,  1.0f } },

		{ { -1.0f,  0.0f,  0.0f }, { -1.0f,  1.0f, -1.0f } },
		{ {  0.0f,  1.0f,  0.0f }, { -1.0f,  1.0f, -1.0f } },
		{ {  0.0f,  0.0f, -1.0f }, { -1.0f,  1.0f, -1.0f } },

		{ { -1.0f,  0.0f,  0.0f }, { -1.0f,  1.0f,  1.0f } },
		{ {  0.0f,  0.0f,  1.0f }, { -1.0f,  1.0f,  1.0f } },
		{ {  0.0f,  1.0f,  0.0f }, { -1.0f,  1.0f,  1.0f } },

		{ { -1.0f,  0.0f,  0.0f }, { -1.0f, -1.0f,  1.0f } },
		{ {  0.0f, -1.0f,  0.0f }, { -1.0f, -1.0f,  1.0f } },
		{ {  0.0f,  0.0f,  1.0f }, { -1.0f, -1.0f,  1.0f } },

		{ { -1.0f,  0.0f,  0.0f }, { -1.0f, -1.0f, -1.0f } },
		{ {  0.0f,  0.0f, -1.0f }, { -1.0f, -1.0f, -1.0f } },
		{ {  0.0f, -1.0f,  0.0f }, { -1.0f, -1.0f, -1.0f } },
	};
	for (const auto& rv : raw_vertices) {
		VertexType v{};
		v.position = { rv.position[0], rv.position[1], rv.position[2] };
		if constexpr (detail::has_normal<VertexType>::value) {
			v.normal = { rv.normal[0], rv.normal[1], rv.normal[2] };
		}
		vertices.push_back(v);
	}

	static const unsigned int raw_indices[] = {
		0, 1, 2,
		3, 4, 5,
		6, 7, 8,
		9, 10, 11,
		12, 13, 14,
		15, 16, 17,
		18, 19, 20,
		21, 22, 23,
	};
	for (auto idx : raw_indices) {
		indices.push_back(static_cast<IndexType>(offset + idx));
	}
}

#endif
