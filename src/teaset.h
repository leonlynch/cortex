/**
 * @file teaset.h
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#ifndef CORTEX_TEASET_H
#define CORTEX_TEASET_H

#include "glm/glm.hpp"

#include "bezier.h"

#include <list>

class Teaset
{
public:
	using BezierPatch = BezierSurface<glm::vec3,3,3>;

protected:
	std::list<BezierPatch> patches;

	virtual ~Teaset();

	void readData(const char* data, bool data_is_ccw);

public:
	template<typename VertexType, typename IndexType = unsigned int>
	void tesselate(unsigned int u_count, unsigned int v_count, std::vector<VertexType>& vertices, std::vector<IndexType>& indices) const;
};

class Teapot : public Teaset
{
public:
	Teapot();
};

class Teacup : public Teaset
{
public:
	Teacup();
};

class Teaspoon : public Teaset
{
public:
	Teaspoon();
};


#include "teaset.tcc"

#endif
