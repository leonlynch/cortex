/**
 * @file teapot.cc
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include "teapot.h"
#include "internal/teapot_geometry.h"

#include <cstddef>
#include <cstdio>

#include <sstream>
#include <string>
#include <array>
#include <vector>

#include <boost/lexical_cast.hpp>

Teapot::Teapot()
{
	readData(teapot_str);
}

Teapot::~Teapot()
{
}

void Teapot::tesselate(unsigned int u_count, unsigned int v_count, std::vector<BezierPatch::Vertex>& vertices, std::vector<unsigned int>& indices) const
{
	for (auto&& patch : patches)
		patch.tesselate(12, 12, vertices, indices);
}

static std::size_t readCount(std::istringstream& ss)
{
	std::string str;
	while (str.empty())
		std::getline(ss, str);

	return boost::lexical_cast<std::size_t>(str);
}

void Teapot::readData(const char* data)
{
	std::istringstream ss(data);
	std::size_t count;
	std::vector<std::array<unsigned int,16>> indices;
	std::vector<glm::vec3> vertices;

	// read indices
	count = readCount(ss);
	indices.resize(count);
	for (std::size_t i = 0; i < count; ++i) {
		std::string index_str;
		std::getline(ss, index_str);

		std::sscanf(
			index_str.c_str(),
			"%u,%u,%u,%u,"
			"%u,%u,%u,%u,"
			"%u,%u,%u,%u,"
			"%u,%u,%u,%u",
			&indices[i][0], &indices[i][1], &indices[i][2], &indices[i][3],
			&indices[i][4], &indices[i][5], &indices[i][6], &indices[i][7],
			&indices[i][8], &indices[i][9], &indices[i][10], &indices[i][11],
			&indices[i][12], &indices[i][13], &indices[i][14], &indices[i][15]
		);
	}

	// read vertices
	count = readCount(ss);
	vertices.resize(count);
	for (std::size_t i = 0; i < count; ++i) {
		std::string vertex_str;
		std::getline(ss, vertex_str);

		std::sscanf(
			vertex_str.c_str(),
			"%f,%f,%f",
			&vertices[i][0], &vertices[i][1], &vertices[i][2]
		);
	}

	// lookup control points
	for (auto&& index : indices) {
		BezierPatch patch;

		for (std::size_t i = 0; i < 16; ++i) {
			patch.k[i%4][i/4] = vertices[index[i] - 1];
		}

		patches.push_back(patch);
	}
}
