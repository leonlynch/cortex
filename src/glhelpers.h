/**
 * @file glhelpers.h
 *
 * Copyright 2026 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#ifndef CORTEX_GLHELPERS_H
#define CORTEX_GLHELPERS_H

#include <GL/glew.h>

/**
 * @brief Determine whether GLSL uniform type is a sampler type.
 *
 * @param type GLSL uniform type as returned by @c glGetActiveUniform()
 * @return Boolean indicating whether @p type is a sampler type.
 */
GLboolean glUniformTypeIsSampler(GLenum type);

#endif
