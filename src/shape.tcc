/**
 * @file shape.tcc
 *
 * Copyright (c) 2020 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include "shape.h"

#ifndef CORTEX_SHAPE_TCC
#define CORTEX_SHAPE_TCC

template <typename T>
template <typename VertexType, typename IndexType>
void Cube<T>::tesselate(std::vector<VertexType>& vertices, std::vector<IndexType>& indices) const
{
	vertices.assign({
		{ {  1.0f,  1.0f,  1.0f }, {  0.0f,  0.0f,  1.0f } },
		{ { -1.0f,  1.0f,  1.0f }, {  0.0f,  0.0f,  1.0f } },
		{ { -1.0f, -1.0f,  1.0f }, {  0.0f,  0.0f,  1.0f } },
		{ {  1.0f, -1.0f,  1.0f }, {  0.0f,  0.0f,  1.0f } },

		{ { -1.0f,  1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f } },
		{ {  1.0f,  1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f } },
		{ {  1.0f, -1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f } },
		{ { -1.0f, -1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f } },

		{ {  1.0f,  1.0f, -1.0f }, {  1.0f,  0.0f,  0.0f } },
		{ {  1.0f,  1.0f,  1.0f }, {  1.0f,  0.0f,  0.0f } },
		{ {  1.0f, -1.0f,  1.0f }, {  1.0f,  0.0f,  0.0f } },
		{ {  1.0f, -1.0f, -1.0f }, {  1.0f,  0.0f,  0.0f } },

		{ { -1.0f,  1.0f,  1.0f }, { -1.0f,  0.0f,  0.0f } },
		{ { -1.0f,  1.0f, -1.0f }, { -1.0f,  0.0f,  0.0f } },
		{ { -1.0f, -1.0f, -1.0f }, { -1.0f,  0.0f,  0.0f } },
		{ { -1.0f, -1.0f,  1.0f }, { -1.0f,  0.0f,  0.0f } },

		{ {  1.0f,  1.0f, -1.0f }, {  0.0f,  1.0f,  0.0f } },
		{ { -1.0f,  1.0f, -1.0f }, {  0.0f,  1.0f,  0.0f } },
		{ { -1.0f,  1.0f,  1.0f }, {  0.0f,  1.0f,  0.0f } },
		{ {  1.0f,  1.0f,  1.0f }, {  0.0f,  1.0f,  0.0f } },

		{ {  1.0f, -1.0f,  1.0f }, {  0.0f, -1.0f,  0.0f } },
		{ { -1.0f, -1.0f,  1.0f }, {  0.0f, -1.0f,  0.0f } },
		{ { -1.0f, -1.0f, -1.0f }, {  0.0f, -1.0f,  0.0f } },
		{ {  1.0f, -1.0f, -1.0f }, {  0.0f, -1.0f,  0.0f } },
	});

	indices.assign({
		0, 1, 3,
		2, 3, 1,

		4, 5, 7,
		6, 7, 5,

		8, 9, 11,
		10, 11, 9,

		12, 13, 15,
		14, 15, 13,

		16, 17, 19,
		18, 19, 17,

		20, 21, 23,
		22, 23, 21,
	});
}

template <typename T>
template <typename VertexType, typename IndexType>
void Octahedron<T>::tesselate(std::vector<VertexType>& vertices, std::vector<IndexType>& indices) const
{
	vertices.assign({
		{ {  1.0f,  0.0f,  0.0f }, {  1.0f,  1.0f,  1.0f } },
		{ {  0.0f,  1.0f,  0.0f }, {  1.0f,  1.0f,  1.0f } },
		{ {  0.0f,  0.0f,  1.0f }, {  1.0f,  1.0f,  1.0f } },

		{ {  1.0f,  0.0f,  0.0f }, {  1.0f,  1.0f, -1.0f } },
		{ {  0.0f,  0.0f, -1.0f }, {  1.0f,  1.0f, -1.0f } },
		{ {  0.0f,  1.0f,  0.0f }, {  1.0f,  1.0f, -1.0f } },

		{ {  1.0f,  0.0f,  0.0f }, {  1.0f, -1.0f, -1.0f } },
		{ {  0.0f, -1.0f,  0.0f }, {  1.0f, -1.0f, -1.0f } },
		{ {  0.0f,  0.0f, -1.0f }, {  1.0f, -1.0f, -1.0f } },

		{ {  1.0f,  0.0f,  0.0f }, {  1.0f, -1.0f,  1.0f } },
		{ {  0.0f,  0.0f,  1.0f }, {  1.0f, -1.0f,  1.0f } },
		{ {  0.0f, -1.0f,  0.0f }, {  1.0f, -1.0f,  1.0f } },

		{ { -1.0f,  0.0f,  0.0f }, { -1.0f,  1.0f, -1.0f } },
		{ {  0.0f,  1.0f,  0.0f }, { -1.0f,  1.0f, -1.0f } },
		{ {  0.0f,  0.0f, -1.0f }, { -1.0f,  1.0f, -1.0f } },

		{ { -1.0f,  0.0f,  0.0f }, { -1.0f,  1.0f,  1.0f } },
		{ {  0.0f,  0.0f,  1.0f }, { -1.0f,  1.0f,  1.0f } },
		{ {  0.0f,  1.0f,  0.0f }, { -1.0f,  1.0f,  1.0f } },

		{ { -1.0f,  0.0f,  0.0f }, { -1.0f, -1.0f,  1.0f } },
		{ {  0.0f, -1.0f,  0.0f }, { -1.0f, -1.0f,  1.0f } },
		{ {  0.0f,  0.0f,  1.0f }, { -1.0f, -1.0f,  1.0f } },

		{ { -1.0f,  0.0f,  0.0f }, { -1.0f, -1.0f, -1.0f } },
		{ {  0.0f,  0.0f, -1.0f }, { -1.0f, -1.0f, -1.0f } },
		{ {  0.0f, -1.0f,  0.0f }, { -1.0f, -1.0f, -1.0f } },
	});

	indices.assign({
		0, 1, 2,
		3, 4, 5,
		6, 7, 8,
		9, 10, 11,
		12, 13, 14,
		15, 16, 17,
		18, 19, 20,
		21, 22, 23,
	});
}

#endif
