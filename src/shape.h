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

#include <vector>

/**
 * @brief Cube template implementation
 *
 * Provides vertex and index data suitable for GL_TRIANGLES rendering. Consists
 * of 24 vertices with per-face flat normals (4 per face) and 12 triangles.
 */
struct Cube
{
	/**
	 * @brief Tessellate the cube into vertex and index buffers suitable for
	 *        GL_TRIANGLES rendering.
	 *
	 * Appends 24 vertices to @p vertices and appends GL_TRIANGLES indices to
	 * @p indices.
	 *
	 * @tparam VertexType 3D vertex type with a @p .position member assignable
	 *                    from three floats. An optional @p .normal member is
	 *                    assigned from three floats when present.
	 * @tparam IndexType Integer type suitable for array indices.
	 *
	 * @param vertices Vertex buffer output. New vertices are appended.
	 * @param indices Index buffer output. New indices are appended.
	 */
	template<typename VertexType, typename IndexType = unsigned int>
	void tessellate(std::vector<VertexType>& vertices, std::vector<IndexType>& indices) const;
};

/**
 * @brief Octahedron template implementation
 *
 * Provides vertex and index data suitable for GL_TRIANGLES rendering. Consists
 * of 24 vertices with per-face flat normals (3 per face) and 8 triangles.
 */
struct Octahedron
{
	/**
	 * @brief Tessellate the octahedron into vertex and index buffers suitable
	 *        for GL_TRIANGLES rendering.
	 *
	 * Appends 24 vertices to @p vertices and appends GL_TRIANGLES indices to
	 * @p indices.
	 *
	 * @tparam VertexType 3D vertex type with a @p .position member assignable
	 *                    from three floats. An optional @p .normal member is
	 *                    assigned from three floats when present.
	 * @tparam IndexType Integer type suitable for array indices.
	 *
	 * @param vertices Vertex buffer output. New vertices are appended.
	 * @param indices Index buffer output. New indices are appended.
	 */
	template<typename VertexType, typename IndexType = unsigned int>
	void tessellate(std::vector<VertexType>& vertices, std::vector<IndexType>& indices) const;
};


#include "shape.tcc"

#endif
