/**
 * @file shape.h
 *
 * Copyright (c) 2020, 2026 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#ifndef CORTEX_SHAPE_H
#define CORTEX_SHAPE_H

#include <cstddef>

#include <vector>

/**
 * @brief Cube template implementation
 *
 * Provides vertex and index data suitable for GL_TRIANGLES rendering. Consists
 * of 24 vertices with per-face flat normals (4 per face) and 12 triangles.
 *
 * @tparam T 3D vector type (e.g. glm::vec3)
 */
template <typename T>
struct Cube
{
	/**
	 * @brief Tessellate the cube into vertex and index buffers suitable for
	 *        GL_TRIANGLES rendering.
	 *
	 * Populates @p vertices and @p indices, replacing any existing content.
	 *
	 * @tparam VertexType 3D Vertex type providing .position and .normal each
	 *                    constructible from three floats.
	 * @tparam IndexType Integer type suitable for array indices
	 *
	 * @param vertices Vertex buffer output. Existing content is replaced.
	 * @param indices Index buffer output. Existing content is replaced.
	 */
	template<typename VertexType, typename IndexType = unsigned int>
	void tessellate(std::vector<VertexType>& vertices, std::vector<IndexType>& indices) const;
};


/**
 * @brief Octahedron template implementation
 *
 * Provides vertex and index data suitable for GL_TRIANGLES rendering. Consists
 * of 24 vertices with per-face flat normals (3 per face) and 8 triangles.
 *
 * @tparam T 3D vector type (e.g. glm::vec3)
 */
template <typename T>
struct Octahedron
{
	/**
	 * @brief Tessellate the octahedron into vertex and index buffers suitable
	 *        for GL_TRIANGLES rendering.
	 *
	 * Populates @p vertices and @p indices, replacing any existing content.
	 *
	 * @tparam VertexType 3D Vertex type providing .position and .normal each
	 *                    constructible from three floats.
	 * @tparam IndexType Integer type suitable for array indices
	 *
	 * @param vertices Vertex buffer output. Existing content is replaced.
	 * @param indices Index buffer output. Existing content is replaced.
	 */
	template<typename VertexType, typename IndexType = unsigned int>
	void tessellate(std::vector<VertexType>& vertices, std::vector<IndexType>& indices) const;
};


#include "shape.tcc"

#endif
