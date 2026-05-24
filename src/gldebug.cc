/**
 * @file gldebug.cc
 *
 * Copyright 2026 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include "gldebug.h"

#include <cstdarg>
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

static const char* gldebug_glsl_type_str(GLenum type)
{
	switch (type) {
		// Scalars
		case GL_FLOAT: return "float";
		case GL_DOUBLE: return "double";
		case GL_INT: return "int";
		case GL_UNSIGNED_INT: return "uint";
		case GL_BOOL: return "bool";

		// Float vectors
		case GL_FLOAT_VEC2: return "vec2";
		case GL_FLOAT_VEC3: return "vec3";
		case GL_FLOAT_VEC4: return "vec4";

		// Double vectors
		case GL_DOUBLE_VEC2: return "dvec2";
		case GL_DOUBLE_VEC3: return "dvec3";
		case GL_DOUBLE_VEC4: return "dvec4";

		// Integer vectors
		case GL_INT_VEC2: return "ivec2";
		case GL_INT_VEC3: return "ivec3";
		case GL_INT_VEC4: return "ivec4";

		// Unsigned int vectors
		case GL_UNSIGNED_INT_VEC2: return "uvec2";
		case GL_UNSIGNED_INT_VEC3: return "uvec3";
		case GL_UNSIGNED_INT_VEC4: return "uvec4";

		// Boolean vectors
		case GL_BOOL_VEC2: return "bvec2";
		case GL_BOOL_VEC3: return "bvec3";
		case GL_BOOL_VEC4: return "bvec4";

		// Float matrices
		case GL_FLOAT_MAT2: return "mat2";
		case GL_FLOAT_MAT3: return "mat3";
		case GL_FLOAT_MAT4: return "mat4";
		case GL_FLOAT_MAT2x3: return "mat2x3";
		case GL_FLOAT_MAT2x4: return "mat2x4";
		case GL_FLOAT_MAT3x2: return "mat3x2";
		case GL_FLOAT_MAT3x4: return "mat3x4";
		case GL_FLOAT_MAT4x2: return "mat4x2";
		case GL_FLOAT_MAT4x3: return "mat4x3";

		// Double matrices
		case GL_DOUBLE_MAT2: return "dmat2";
		case GL_DOUBLE_MAT3: return "dmat3";
		case GL_DOUBLE_MAT4: return "dmat4";
		case GL_DOUBLE_MAT2x3: return "dmat2x3";
		case GL_DOUBLE_MAT2x4: return "dmat2x4";
		case GL_DOUBLE_MAT3x2: return "dmat3x2";
		case GL_DOUBLE_MAT3x4: return "dmat3x4";
		case GL_DOUBLE_MAT4x2: return "dmat4x2";
		case GL_DOUBLE_MAT4x3: return "dmat4x3";

		// Float samplers
		case GL_SAMPLER_1D: return "sampler1D";
		case GL_SAMPLER_2D: return "sampler2D";
		case GL_SAMPLER_3D: return "sampler3D";
		case GL_SAMPLER_CUBE: return "samplerCube";
		case GL_SAMPLER_1D_SHADOW: return "sampler1DShadow";
		case GL_SAMPLER_2D_SHADOW: return "sampler2DShadow";
		case GL_SAMPLER_1D_ARRAY: return "sampler1DArray";
		case GL_SAMPLER_2D_ARRAY: return "sampler2DArray";
		case GL_SAMPLER_1D_ARRAY_SHADOW: return "sampler1DArrayShadow";
		case GL_SAMPLER_2D_ARRAY_SHADOW: return "sampler2DArrayShadow";
		case GL_SAMPLER_2D_MULTISAMPLE: return "sampler2DMS";
		case GL_SAMPLER_2D_MULTISAMPLE_ARRAY: return "sampler2DMSArray";
		case GL_SAMPLER_CUBE_SHADOW: return "samplerCubeShadow";
		case GL_SAMPLER_CUBE_MAP_ARRAY: return "samplerCubeArray";
		case GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW: return "samplerCubeArrayShadow";
		case GL_SAMPLER_BUFFER: return "samplerBuffer";
		case GL_SAMPLER_2D_RECT: return "sampler2DRect";
		case GL_SAMPLER_2D_RECT_SHADOW: return "sampler2DRectShadow";

		// Integer samplers
		case GL_INT_SAMPLER_1D: return "isampler1D";
		case GL_INT_SAMPLER_2D: return "isampler2D";
		case GL_INT_SAMPLER_3D: return "isampler3D";
		case GL_INT_SAMPLER_CUBE: return "isamplerCube";
		case GL_INT_SAMPLER_1D_ARRAY: return "isampler1DArray";
		case GL_INT_SAMPLER_2D_ARRAY: return "isampler2DArray";
		case GL_INT_SAMPLER_2D_MULTISAMPLE: return "isampler2DMS";
		case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: return "isampler2DMSArray";
		case GL_INT_SAMPLER_BUFFER: return "isamplerBuffer";
		case GL_INT_SAMPLER_2D_RECT: return "isampler2DRect";
		case GL_INT_SAMPLER_CUBE_MAP_ARRAY: return "isamplerCubeArray";

		// Unsigned integer samplers
		case GL_UNSIGNED_INT_SAMPLER_1D: return "usampler1D";
		case GL_UNSIGNED_INT_SAMPLER_2D: return "usampler2D";
		case GL_UNSIGNED_INT_SAMPLER_3D: return "usampler3D";
		case GL_UNSIGNED_INT_SAMPLER_CUBE: return "usamplerCube";
		case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY: return "usampler1DArray";
		case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY: return "usampler2DArray";
		case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE: return "usampler2DMS";
		case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: return "usampler2DMSArray";
		case GL_UNSIGNED_INT_SAMPLER_BUFFER: return "usamplerBuffer";
		case GL_UNSIGNED_INT_SAMPLER_2D_RECT: return "usampler2DRect";
		case GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY: return "usamplerCubeArray";

		default:
			return "unknown";
	}
}

void cortex_gldebug_callback(
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
	printf("%s[GL|%s|%s|%u]: %s\n" ANSI_RESET,
		gldebug_severity_str(severity),
		gldebug_source_str(source),
		gldebug_type_str(type),
		id,
		message
	);
}

void cortex_gldebug_log(GLenum severity, const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	printf("%s[GL|%s]: ",
		gldebug_severity_str(severity),
		gldebug_source_str(GL_DEBUG_SOURCE_APPLICATION)
	);
	vprintf(fmt, ap);
	printf("\n" ANSI_RESET);
	va_end(ap);
}

void cortex_gldebug_uniform(
	GLenum severity,
	const char* name,
	GLint size,
	GLenum type,
	GLint location
)
{
	if (size > 1) {
		printf("%s[GL|shader] layout(location=%d) uniform %s %s[%d];\n" ANSI_RESET,
			gldebug_severity_str(severity),
			location,
			gldebug_glsl_type_str(type),
			name,
			size
		);
	} else {
		printf("%s[GL|shader] layout(location=%d) uniform %s %s;\n" ANSI_RESET,
			gldebug_severity_str(severity),
			location,
			gldebug_glsl_type_str(type),
			name
		);
	}
}

void cortex_gldebug_attribute(
	GLenum severity,
	const char* name,
	GLint size,
	GLenum type,
	GLint location
)
{
	if (size > 1) {
		printf("%s[GL|shader] layout(location=%d) in %s %s[%d];\n" ANSI_RESET,
			gldebug_severity_str(severity),
			location,
			gldebug_glsl_type_str(type),
			name,
			size
		);
	} else {
		printf("%s[GL|shader] layout(location=%d) in %s %s;\n" ANSI_RESET,
			gldebug_severity_str(severity),
			location,
			gldebug_glsl_type_str(type),
			name
		);
	}
}

void cortex_gldebug_sampler(
	GLenum severity,
	const char* name,
	GLint size,
	GLenum type,
	GLint location,
	GLint unit
)
{
	if (size > 1) {
		printf("%s[GL|shader] layout(binding=%d) uniform %s %s[%d]; // location=%d\n" ANSI_RESET,
			gldebug_severity_str(severity),
			unit,
			gldebug_glsl_type_str(type),
			name,
			size,
			location
		);
	} else {
		printf("%s[GL|shader] layout(binding=%d) uniform %s %s; // location=%d\n" ANSI_RESET,
			gldebug_severity_str(severity),
			unit,
			gldebug_glsl_type_str(type),
			name,
			location
		);
	}
}

void cortex_gldebug_fragdata(
	GLenum severity,
	const char* name,
	GLint size,
	GLenum type,
	GLint location
)
{
	if (size > 1) {
		printf("%s[GL|shader] layout(location=%d) out %s %s[%d];\n" ANSI_RESET,
			gldebug_severity_str(severity),
			location,
			gldebug_glsl_type_str(type),
			name,
			size
		);
	} else {
		printf("%s[GL|shader] layout(location=%d) out %s %s;\n" ANSI_RESET,
			gldebug_severity_str(severity),
			location,
			gldebug_glsl_type_str(type),
			name
		);
	}
}
