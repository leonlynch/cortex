/**
 * @file textured.frag.glsl
 *
 * Copyright 2026 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#version 460 core

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
	float shininess;
};
uniform material_t material;
layout(binding=0) uniform sampler2D material_diffuse;
layout(binding=1) uniform sampler2D material_specular;

in vec3 f_n;
in vec3 f_l;
in vec3 f_v;
in vec2 f_texcoord;

out vec3 color;

void main()
{
	// Normalize eye space vectors
	vec3 n = normalize(f_n);
	vec3 l = normalize(f_l);
	vec3 v = normalize(f_v);

	// Sample material colors from textures
	vec3 diff_color = texture(material_diffuse, f_texcoord).rgb;
	vec3 spec_color = texture(material_specular, f_texcoord).rgb;

	// Compute ambient and diffuse lighting
	float diffuse_intensity = max(dot(n, l), 0.0);
	vec3 ambient = light.ambient * diff_color;
	vec3 diffuse = light.diffuse * diff_color * diffuse_intensity;

	// Compute specular lighting
	vec3 specular = vec3(0.0);
	if (diffuse_intensity > 0.0) {
		vec3 h = normalize(l + v);
		float specular_intensity = max(dot(h, n), 0.0);
		specular = light.specular * spec_color * pow(specular_intensity, material.shininess);
	}

	// Compute color
	color = max(diffuse + specular, ambient);
}
