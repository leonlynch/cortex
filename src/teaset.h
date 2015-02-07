/**
 * @file teaset.h
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#ifndef __CORTEX_TEASET_H__
#define __CORTEX_TEASET_H__

#include "glm/glm.hpp"

#include "bezier.h"

#include <list>

class Teaset
{
public:
	typedef BezierSurface<glm::vec3,3,3> BezierPatch;

protected:
	std::list<BezierPatch> patches;

	virtual ~Teaset();

	void readData(const char* data, bool data_is_ccw);

public:
	void tesselate(unsigned int u_count, unsigned int v_count, std::vector<BezierPatch::Vertex>& vertices, std::vector<unsigned int>& indices) const;
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

#endif
