/**
 * @file sceneloader.h
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#ifndef CORTEX_SCENE_LOADER_H
#define CORTEX_SCENE_LOADER_H

#include <string>

// Forward declaration
class Entity;

class SceneLoader
{
public:
	SceneLoader(bool logger = false, bool verbose = false);
	virtual ~SceneLoader();

	SceneLoader(const SceneLoader&) = delete;
	SceneLoader& operator=(const SceneLoader&) = delete;

	Entity* createEntityFromFile(const std::string& name, const std::string& filename);
};

#endif
