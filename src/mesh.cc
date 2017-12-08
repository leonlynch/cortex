/**
 * @file mesh.cc
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include "mesh.h"

Mesh::Mesh(
	PrimitiveType primitive_type,
	std::size_t vertex_count,
	bool has_normals,
	bool has_tangents,
	bool has_bitangents,
	bool has_colors
)
: primitive_type(primitive_type),
  vertex_size(3)
{
	normal_size = has_normals ? 3 : 0;
	tangent_size = has_tangents ? 3 : 0;
	bitangent_size = has_bitangents ? 3 : 0;
	color_size = has_colors ? 4 : 0;
	stride = vertex_size + normal_size + tangent_size + bitangent_size + color_size;

	vertex_data.reserve(vertex_count * stride);
}
