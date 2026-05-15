/**
 * @file teaset.tcc
 *
 * Copyright (c) 2013, 2026 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include "teaset.h"

#ifndef CORTEX_TEASET_TCC
#define CORTEX_TEASET_TCC

template<typename VertexType, typename IndexType>
void Teaset::tessellate(unsigned int u_count, unsigned int v_count, std::vector<VertexType>& vertices, std::vector<IndexType>& indices) const
{
	for (auto&& patch : patches) {
		patch.tessellate(u_count, v_count, vertices, indices);
	}
}

#endif
