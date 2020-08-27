/**
 * @file sphere.tcc
 *
 * Copyright (c) 2020 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include "sphere.h"

#ifndef CORTEX_SPHERE_TCC
#define CORTEX_SPHERE_TCC

#include <glm/glm.hpp>

template <typename T, typename IndexType>
void subdivide(std::size_t divisions, std::vector<T>& vertices, std::vector<IndexType>& indices, const IndexType* idx)
{
	if (divisions > 0) {
		// lookup vertices of the current triangle
		T v[] = {
			vertices[idx[0]],
			vertices[idx[1]],
			vertices[idx[2]],
		};

		// compute halfway vertices
		T h[] = {
			glm::normalize((v[0] + v[1]) * 0.5f),
			glm::normalize((v[1] + v[2]) * 0.5f),
			glm::normalize((v[2] + v[0]) * 0.5f),
		};
		
		// store halfway vertices
		vertices.insert(vertices.end(), std::begin(h), std::end(h));

		// compute halfway indices
		IndexType h_idx[] = {
			static_cast<IndexType>(vertices.size() - 3),
			static_cast<IndexType>(vertices.size() - 2),
			static_cast<IndexType>(vertices.size() - 1),
		};

		// middle triangle
		subdivide(divisions - 1, vertices, indices, h_idx);
		
		// corner triangles
		IndexType t1_idx[] = { idx[0], h_idx[0], h_idx[2] };
		subdivide(divisions - 1, vertices, indices, t1_idx);
		IndexType t2_idx[] = { idx[1], h_idx[1], h_idx[0] };
		subdivide(divisions - 1, vertices, indices, t2_idx);
		IndexType t3_idx[] = { idx[2], h_idx[2], h_idx[1] };
		subdivide(divisions - 1, vertices, indices, t3_idx);
	} else {
		// add indices for current triangle
		indices.insert(indices.end(), idx, idx + 3);
	}
}

template <typename T>
template <typename VertexType, typename IndexType>
void Sphere<T>::tesselate(std::size_t divisions, std::vector<VertexType>& vertices, std::vector<IndexType>& indices) const
{
	T v[] = { 
		{ 1, 0, 0 },
		{ 0, 1, 0 },
		{ 0, 0, 1 },
	};
	IndexType idx[] = { 0, 1, 2 };
	
	// create initial surface; 8th of octahedron
	std::vector<T> vectors;
	vectors.insert(vectors.end(), std::begin(v), std::end(v));
	subdivide(divisions, vectors, indices, idx);

	// create quarter of octahedron
	{
		// build transformation matrix rotation around x-axis
		glm::mat3 transform(0.0f);
		transform[0][0] = 1.0f;
		transform[1][2] = 1.0f;
		transform[2][1] = -1.0f;
		
		// append rotated vectors
		std::size_t vector_count = vectors.size();
		for (std::size_t i = 0; i < vector_count; ++i) {
			vectors.push_back(transform * vectors[i]);
		}
		std::size_t index_count = indices.size();
		for (std::size_t i = 0; i < index_count; ++i) {
			indices.push_back(indices[i] + vector_count);
		}
	}

	// create half of octahedron
	{
		// build transformation matrix rotation around x-axis
		glm::mat3 transform(0.0f);
		transform[0][0] = 1.0f;
		transform[1][1] = -1.0f;
		transform[2][2] = -1.0f;
		
		// append rotated vectors
		std::size_t vector_count = vectors.size();
		for (std::size_t i = 0; i < vector_count; ++i) {
			vectors.push_back(transform * vectors[i]);
		}
		std::size_t index_count = indices.size();
		for (std::size_t i = 0; i < index_count; ++i) {
			indices.push_back(indices[i] + vector_count);
		}
	}
	
	// create full octahedron
	{
		// build transformation matrix rotation around z-axis
		glm::mat3 transform(0.0f);
		transform[0][0] = -1.0f;
		transform[1][1] = -1.0f;
		transform[2][2] = 1.0f;
		
		// append rotated vectors
		std::size_t vector_count = vectors.size();
		for (std::size_t i = 0; i < vector_count; ++i) {
			vectors.push_back(transform * vectors[i]);
		}
		std::size_t index_count = indices.size();
		for (std::size_t i = 0; i < index_count; ++i) {
			indices.push_back(indices[i] + vector_count);
		}
	}

	vertices.reserve(vectors.size());
	for (auto&& v : vectors) {

		VertexType vertex;
		vertex.position = v;
		vertex.normal = glm::normalize(v);
		vertices.push_back(std::move(vertex));
	}
}

#endif
