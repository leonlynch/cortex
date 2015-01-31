/**
 * @file teapot.h
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#ifndef __CORTEX_TEAPOT_H__
#define __CORTEX_TEAPOT_H__

#include "glm/glm.hpp"

#include "bezier.h"

#include <list>

class Teapot
{
public:
	typedef BezierSurface<glm::vec3,3,3> BezierPatch;

private:
	std::list<BezierPatch> patches;

public:
	Teapot();
	virtual ~Teapot();

	void tesselate(unsigned int u_count, unsigned int v_count, std::vector<BezierPatch::Vertex>& vertices, std::vector<unsigned int>& indices) const;

private:
	void readData(const char* data);
};

#endif
