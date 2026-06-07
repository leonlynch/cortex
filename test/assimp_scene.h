/**
 * @file assimp_scene.h
 *
 * Copyright 2026 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#ifndef ASSIMP_SCENE_H
#define ASSIMP_SCENE_H

#include <stdbool.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

int scene_init(void);

int scene_load_resources(void);

void scene_unload_resources(void);

void scene_resize(int width, int height);

void scene_update(void);

void scene_render(void);

void scene_set_wireframe(bool enabled);

void scene_rotate(float delta_yaw, float delta_pitch);

void scene_zoom(float delta);

__END_DECLS

#endif
