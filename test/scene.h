/**
 * @file scene.h
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#ifndef __TEST_SCENE_H__
#define __TEST_SCENE_H__

#include <sys/cdefs.h>

__BEGIN_DECLS

int scene_init(void);

int scene_load_resources(void);

void scene_unload_resources(void);

void scene_resize(int width, int height);

void scene_update(void);

void scene_render(void);

__END_DECLS

#endif
