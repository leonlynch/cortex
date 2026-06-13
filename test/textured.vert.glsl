/**
 * @file textured.vert.glsl
 *
 * Copyright 2026 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#version 460 core

uniform mat4 m_modelview;
uniform mat3 m_normal;
uniform mat4 m_view;
uniform mat4 m_mvp;

struct light_t {
	vec4 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform light_t light;

struct material_t {
	float shininess;
};
uniform material_t material;

in vec3 v_position;
in vec3 v_normal;
in vec2 v_texcoord;

out vec3 f_n;
out vec3 f_l;
out vec3 f_v;
out vec2 f_texcoord;

void main()
{
	// Compute eye space vectors
	vec4 position = m_modelview * vec4(v_position, 1.0);
	f_n = m_normal * v_normal; // Normal vector
	f_l = vec3(m_view * light.position - position); // Light vector
	f_v = vec3(-position); // Viewer vector

	f_texcoord = v_texcoord;

	gl_Position = m_mvp * vec4(v_position, 1.0);
}
