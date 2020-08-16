/**
 * @file simple.frag.glsl
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#version 330 core

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

in vec3 f_n;
in vec3 f_l;
in vec3 f_v;

out vec3 color;

void main()
{
	// normalize eye space vectors
	vec3 n = normalize(f_n);
	vec3 l = normalize(f_l);
	vec3 v = normalize(f_v);

	// compute ambient and diffuse lighting
	float diffuse_intensity = max(dot(n, l), 0.0);
	vec3 ambient = light.ambient * material.ambient;
	vec3 diffuse = light.diffuse * material.diffuse * diffuse_intensity;

	// compute specular lighting
	vec3 specular = vec3(0.0);
	if (diffuse_intensity > 0.0) {
		vec3 h = normalize(l + v);
		float specular_intensity = max(dot(h, n), 0.0);
		specular = light.specular * material.specular * pow(specular_intensity, material.shininess);
	}

	// compute color
	color = max(diffuse + specular, ambient);
}
