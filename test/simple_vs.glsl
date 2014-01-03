/**
 * @file simple_vs.glsl
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#version 130

uniform mat4 mvp;

in vec4 position;
in vec4 color;
out vec4 v_color;

void main()
{
	gl_Position = mvp * position;
	v_color = color;
}
