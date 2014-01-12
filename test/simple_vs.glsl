/**
 * @file simple_vs.glsl
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#version 130

uniform mat4 m_mvp;
uniform mat4 m_modelview;
uniform mat3 m_normal;

struct light_t {
	vec4 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform light_t light;

struct material_t {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
uniform material_t material;

in vec3 v_position;
in vec3 v_normal;

out vec3 f_n;
out vec3 f_l;
out vec3 f_v;

void main()
{
	// compute eye space vectors
	vec4 position = m_modelview * vec4(v_position, 1.0);
	f_n = m_normal * v_normal; // normal vector
	f_l = vec3(light.position - position); // light vector
	f_v = vec3(-position); // viewer vector

	gl_Position = m_mvp * vec4(v_position, 1.0);
}
