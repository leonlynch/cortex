/**
 * @file sphere.h
 *
 * Copyright (c) 2020, 2026 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#ifndef CORTEX_SPHERE_H
#define CORTEX_SPHERE_H

#include <cstddef>
#include <vector>

/**
 * @brief Unit sphere tessellation template implementation
 *
 * Builds a unit sphere by recursively subdividing an octahedron. Computes
 * vertex and index data suitable for GL_TRIANGLES rendering.
 *
 * @tparam T 3D vector type (e.g. glm::vec3) used for internal position
 *           computation. Must support glm::normalize() and arithmetic
 *           operators.
 */
template <typename T>
struct Sphere
{
	/**
	 * @brief Tessellate the sphere into vertex and index buffers suitable for
	 *        GL_TRIANGLES rendering.
	 *
	 * Subdivides each octahedron face @p divisions times, appending vertices
	 * to @p vertices and GL_TRIANGLES indices to @p indices. Each level
	 * multiplies the triangle count by 4; @p divisions = 0 produces an
	 * octahedron (8 triangles).
	 *
	 * @tparam VertexType 3D vertex type with a @p .position member assignable
	 *                    from T. An optional @p .normal member is assigned
	 *                    from T when present.
	 * @tparam IndexType Integer type suitable for array indices.
	 *
	 * @param divisions Number of subdivision levels. Must be >= 0.
	 * @param vertices Vertex buffer output. New vertices are appended.
	 * @param indices Index buffer output. New indices are appended.
	 */
	template<typename VertexType, typename IndexType = unsigned int>
	void tessellate(std::size_t divisions, std::vector<VertexType>& vertices, std::vector<IndexType>& indices) const;
};


#include "sphere.tcc"

#endif
