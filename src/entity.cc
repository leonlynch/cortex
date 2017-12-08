/**
 * @file entity.cc
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include "entity.h"
#include "mesh.h"

Entity::Entity(const std::string name)
: name(name)
{
}

Entity::~Entity()
{
	for (auto&& itr : meshes)
		delete itr;
}
