/**
 * @file testscene.h
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#ifndef TEST_SCENE_H
#define TEST_SCENE_H

#include <stdbool.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

enum scene_demo_t {
	SCENE_DEMO_CUBE,
	SCENE_DEMO_OCTAHEDRON,
	SCENE_DEMO_BEZIER,
	SCENE_DEMO_TEAPOT,
	SCENE_DEMO_TEACUP,
	SCENE_DEMO_TEASPOON,
	SCENE_DEMO_SPHERE,
};

int scene_init(void);

int scene_load_resources(void);

void scene_unload_resources(void);

void scene_resize(int width, int height);

void scene_update(void);

void scene_render(enum scene_demo_t scene_demo);

enum scene_demo_t scene_next_demo(enum scene_demo_t current_demo);

void scene_set_normals(bool enabled);

void scene_set_wireframe(bool enabled);

__END_DECLS

#endif
