/**
 * @file teaset.cc
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include "teaset.h"

#include <cstddef>
#include <cstdio>

#include <sstream>
#include <string>
#include <array>
#include <vector>

#include <boost/lexical_cast.hpp>

extern const char teapot_geometry_str[];
extern const char teacup_geometry_str[];
extern const char teaspoon_geometry_str[];

Teaset::~Teaset()
{
}

static std::size_t readCount(std::istringstream& ss)
{
	std::string str;
	while (str.empty())
		std::getline(ss, str);

	return boost::lexical_cast<std::size_t>(str);
}

void Teaset::readData(const char* data, bool data_is_ccw)
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

		if (data_is_ccw) {
			for (std::size_t i = 0; i < 16; ++i) {
				patch.k[i/4][i%4] = vertices[index[i] - 1];
			}
		} else {
			for (std::size_t i = 0; i < 16; ++i) {
				patch.k[i/4][3 - (i%4)] = vertices[index[i] - 1];
			}
		}

		patches.push_back(patch);
	}
}

Teapot::Teapot()
{
	readData(teapot_geometry_str, false);
}

Teacup::Teacup()
{
	readData(teacup_geometry_str, true);
}

Teaspoon::Teaspoon()
{
	readData(teaspoon_geometry_str, true);
}
