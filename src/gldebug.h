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

void cortex_gldebug_callback(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam
);

void cortex_gldebug_log(
	GLenum severity,
	const char* fmt, ...
) __attribute__((format(printf, 2, 3)));

template<typename... Args>
static inline void cortex_gldebug_msg(const char* fmt, Args... args)
{
	cortex_gldebug_log(GL_DEBUG_SEVERITY_NOTIFICATION, fmt, args...);
}

template<typename... Args>
static inline void cortex_gldebug_error(const char* fmt, Args... args)
{
	cortex_gldebug_log(GL_DEBUG_SEVERITY_HIGH, fmt, args...);
}

void cortex_gldebug_uniform(
	GLenum severity,
	const char* name,
	GLint size,
	GLenum type,
	GLint location
);

static inline void cortex_gldebug_uniform(
	const char* name,
	GLint size,
	GLenum type,
	GLint location
) {
	cortex_gldebug_uniform(GL_DEBUG_SEVERITY_NOTIFICATION,
		name,
		size,
		type,
		location
	);
}

void cortex_gldebug_attribute(
	GLenum severity,
	const char* name,
	GLint size,
	GLenum type,
	GLint location
);

static inline void cortex_gldebug_attribute(
	const char* name,
	GLint size,
	GLenum type,
	GLint location
) {
	cortex_gldebug_attribute(GL_DEBUG_SEVERITY_NOTIFICATION,
		name,
		size,
		type,
		location
	);
}

void cortex_gldebug_fragdata(
	GLenum severity,
	const char* name,
	GLint size,
	GLenum type,
	GLint location
);

static inline void cortex_gldebug_fragdata(
	const char* name,
	GLint size,
	GLenum type,
	GLint location
) {
	cortex_gldebug_fragdata(GL_DEBUG_SEVERITY_NOTIFICATION,
		name,
		size,
		type,
		location
	);
}

#endif
