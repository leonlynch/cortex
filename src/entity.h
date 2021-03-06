/**
 * @file entity.h
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#ifndef CORTEX_ENTITY
#define CORTEX_ENTITY

#include <string>
#include <vector>
#include <list>

// Forward declarations
class Material;
class Mesh;

class Entity
{
public:
	std::string name;
	std::vector<Material*> materials;
	std::list<Mesh*> meshes;

public:
	Entity(const std::string name);
	virtual ~Entity();

	Entity(const Entity&) = delete;
	Entity& operator=(const Entity&) = delete;
};

#endif
