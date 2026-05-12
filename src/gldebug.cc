/**
 * @file gldebug.cc
 *
 * Copyright 2026 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include "gldebug.h"

#include <cstdio>

#define ANSI_FG_RED     "\x1b[31m"
#define ANSI_FG_YELLOW  "\x1b[33m"
#define ANSI_FG_BLUE    "\x1b[34m"
#define ANSI_FG_DIM     "\x1b[2m"
#define ANSI_BG_RED     "\x1b[41m"
#define ANSI_RESET      "\x1b[0m"

static const char* gldebug_source_str(GLenum source)
{
	switch (source) {
		case GL_DEBUG_SOURCE_API:
			return "API";

		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			return "window system";

		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			return "shader compiler";

		case GL_DEBUG_SOURCE_THIRD_PARTY:
			return "third party";

		case GL_DEBUG_SOURCE_APPLICATION:
			return "application";

		case GL_DEBUG_SOURCE_OTHER:
			return "other";

		default:
			return "unknown";
	}
}

static const char* gldebug_type_str(GLenum type)
{
	switch (type) {
		case GL_DEBUG_TYPE_ERROR:
			return "error";

		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			return "deprecated";

		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			return "undefined behaviour";

		case GL_DEBUG_TYPE_PORTABILITY:
			return "portability";

		case GL_DEBUG_TYPE_PERFORMANCE:
			return "performance";

		case GL_DEBUG_TYPE_MARKER:
			return "marker";

		case GL_DEBUG_TYPE_PUSH_GROUP:
			return "push group";

		case GL_DEBUG_TYPE_POP_GROUP:
			return "pop group";

		case GL_DEBUG_TYPE_OTHER:
			return "other";

		default:
			return "unknown";
	}
}

static const char* gldebug_severity_str(GLenum severity)
{
	switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH:
			return ANSI_FG_RED;

		case GL_DEBUG_SEVERITY_MEDIUM:
			return ANSI_FG_YELLOW;

		case GL_DEBUG_SEVERITY_LOW:
			return ANSI_FG_BLUE;

		case GL_DEBUG_SEVERITY_NOTIFICATION:
			return ANSI_FG_DIM;

		default:
			return ANSI_BG_RED;
	}
}

void cortex_gldebug(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam
)
{
	(void)length;
	(void)userParam;
	fprintf(stderr, "%s[GL|%s|%s|%u]: %s\n" ANSI_RESET,
		gldebug_severity_str(severity),
		gldebug_source_str(source),
		gldebug_type_str(type),
		id,
		message
	);
}
