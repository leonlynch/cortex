/**
 * @file shape.h
 *
 * Copyright (c) 2020 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#ifndef CORTEX_SHAPE_H
#define CORTEX_SHAPE_H

#include <cstddef>

#include <vector>

template <typename T>
struct Cube
{
	template<typename VertexType, typename IndexType = unsigned int>
	void tesselate(std::vector<VertexType>& vertices, std::vector<IndexType>& indices) const;
};


template <typename T>
struct Octahedron
{
	template<typename VertexType, typename IndexType = unsigned int>
	void tesselate(std::vector<VertexType>& vertices, std::vector<IndexType>& indices) const;
};


#include "shape.tcc"

#endif
