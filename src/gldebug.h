/**
 * @file gldebug.h
 *
 * Copyright 2026 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#ifndef CORTEX_GLDEBUG_H
#define CORTEX_GLDEBUG_H

#include <GL/glew.h>

void cortex_gldebug(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam
);

#endif
