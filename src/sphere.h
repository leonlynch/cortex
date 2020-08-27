/**
 * @file sphere.h
 *
 * Copyright (c) 2020 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#ifndef CORTEX_SPHERE_H
#define CORTEX_SPHERE_H

#include <cstddef>

#include <vector>

template <typename T>
struct Sphere
{
	template<typename VertexType, typename IndexType = unsigned int>
	void tesselate(std::size_t divisions, std::vector<VertexType>& vertices, std::vector<IndexType>& indices) const;
};


#include "sphere.tcc"

#endif
