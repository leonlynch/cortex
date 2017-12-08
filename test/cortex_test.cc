/**
 * @file cortex_test.c
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include "sceneloader.h"
#include "entity.h"

#include <cstdio>

int main(int argc, char** argv)
{
	if (argc != 2) {
		std::fprintf(stderr, "No import file specified\n");
		return 1;
	}

	SceneLoader loader;
	Entity* entity;

	entity = loader.createEntityFromFile("test", argv[1]);
	if (!entity) {
		std::fprintf(stderr, "Failed to import '%s'", argv[1]);
		return 1;
	}

	delete entity;

	return 0;
}
