/**
 * @file mesh.h
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#ifndef CORTEX_MESH
#define CORTEX_MESH

#include <cstddef>
#include <vector>

// Forward declaration
class Material;

class Mesh
{
public:
	enum class PrimitiveType {
		Point = 1,
		Line = 2,
		Triangle = 3,
	};

public:
	// populated by constructor
	PrimitiveType primitive_type;
	std::size_t vertex_size;
	std::size_t normal_size;
	std::size_t tangent_size;
	std::size_t bitangent_size;
	std::size_t color_size;
	std::size_t stride;

	// populated by caller
	std::vector<float> vertex_data;
	std::vector<unsigned int> index_data;
	Material* material;

public:
	/**
	 * Constructor
	 * @param primitive_type Primitive type in mesh
	 * @param vertex_count Number of vertices
	 * @param normal_present Boolean indicating whether vertices have normals
	 * @param has_tangents Boolean indicating whether vertices have tangents
	 * @param has_bitangents Boolean indicating whether vertices have bitangents
	 * @param has_colors Boolean indicating whether vertices have colors
	 *
	 * @note This will reserve the appropriate amount of space in @ref vertex_data
	 */
	Mesh(
		PrimitiveType primitive_type,
		std::size_t vertex_count,
		bool has_normals = false,
		bool has_tangents = false,
		bool has_bitangents = false,
		bool has_colors = false
	);

	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;
};

#endif
