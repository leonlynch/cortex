/**
 * @file pbr.frag.glsl
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

layout(binding=0) uniform sampler2D material_albedo;
layout(binding=1) uniform sampler2D material_arm;
layout(binding=2) uniform sampler2D material_normal;

in vec3 f_l;
in vec3 f_v;
in mat3 f_tbn;
in vec2 f_texcoord;

out vec3 color;

// Useful constants
const float PI = 3.14159265359;
const float EPSILON = 1e-4; // Very small number to prevent divide-by-zero

// Trowbridge-Reitz GGX normal distribution function (NDF):
// - Returns fraction of microfacets whose half vector H is aligned with N
// - Alpha = roughness^2 is the Disney roughness remapping to make the roughness
//   range perceptually linear.
// Reference: "Physically Based Shading at Disney" (Burley 2012)
float D_GGX(float NdotH, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float d = NdotH * NdotH * (a2 - 1.0) + 1.0;
	return a2 / (PI * d * d);
}

// Smith-Schlick-GGX geometry function:
// - Returns probability that a microfacet is not self-shadowed or self-masked
// - k = (roughness+1)^2/8 is for direct lights, not image-based lighting (IBL)
// Reference: "Real Shading in Unreal Engine 4" (Karis 2013).
float G_Schlick_GGX(float NdotX, float roughness)
{
	float k = (roughness + 1.0);
	k = (k * k) / 8.0;
	return NdotX / (NdotX * (1.0 - k) + k);
}

// Smith combined geometry function
// Applies G_Schlick_GGX for viewer, to account for self-masking (view angle),
// and for light to account for self-shadowing (light angle)
float G_Smith(float NdotV, float NdotL, float roughness)
{
	return G_Schlick_GGX(NdotV, roughness) * G_Schlick_GGX(NdotL, roughness);
}

// Fresnel-Schlick approximation function:
// - Returns fraction of light reflected (vs refracted) at the surface
// - F0 is the base reflectance at normal incidence (0 degrees):
//   - ~0.04 for non-metals (dielectrics)
//   - albedo for metals (conductors, since absorbed light is not re-emitted)
// - At grazing angles the reflectance rises toward 1.0 (via (1-HdotV)^5 term)
// Reference: "An Inexpensive BRDF Model for Physically-based Rendering" (Schlick 1994).
vec3 F_Schlick(float HdotV, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - HdotV, 5.0);
}

void main()
{
	// Sample textures and unpack components
	vec3 albedo = texture(material_albedo, f_texcoord).rgb;
	vec3 arm = texture(material_arm, f_texcoord).rgb;
	float ao        = arm.r;
	float roughness = arm.g;
	float metalness = arm.b;

	// Compute eye space normal from tangent-space normal map:
	// - Sample tangent-space normal map and remap [0,1] to [-1,1]
	// - Transform to eye space via the TBN matrix
	vec3 normal_ts = texture(material_normal, f_texcoord).rgb * 2.0 - 1.0;
	vec3 n = normalize(f_tbn * normal_ts);

	// Normalize eye space vectors:
	// - L: surface-to-light vector
	// - V: surface-to-viewer vector
	// - H: half vector (bisects L and V)
	vec3 l = normalize(f_l);
	vec3 v = normalize(f_v);
	vec3 h = normalize(l + v);

	// Compute various angle cosines used by BRDF:
	// - NdotL governs Lambert's law (irradiance scaling by angle of light)
	// - NdotV used by G in BRDF and clamped to avoid precision issues for
	//   large D (smooth material, grazing view angle) and tiny NdotV
	// - NdotH selects the fraction of microfacets facing H (used by D in BRDF)
	// - HdotV used by F in BRDF (reflectance at the half-angle).
	float NdotL = max(dot(n, l), 0.0);
	float NdotV = max(dot(n, v), EPSILON);
	float NdotH = max(dot(n, h), 0.0);
	float HdotV = max(dot(h, v), 0.0);

	// Compute metallic reflection
	// - Use 0.04 reflectance for non-metals (common dielectric approximation)
	// - Use albedo for metals (conductors reflect their own color)
	vec3 F0 = mix(vec3(0.04), albedo, metalness);

	// Cook-Torrance microfacet specular BRDF: D*G*F / (4*NdotV*NdotL)
	// - D: fraction of microfacets oriented toward H (sharpness of highlight)
	// - G: microfacet self-masking and self-shadowing
	// - F: fraction of light that is reflected (vs refracted)
	float D = D_GGX(NdotH, roughness);
	float G = G_Smith(NdotV, NdotL, roughness);
	vec3  F = F_Schlick(HdotV, F0);
	vec3 specular = (D * G * F) / (4.0 * NdotV * NdotL + EPSILON);

	// Lambertian diffuse BRDF: weighted albedo / PI
	// - kd weights the diffuse term by what Fresnel did not reflect
	// - Metals absorb refracted (diffuse) light
	vec3 kd = (1.0 - F) * (1.0 - metalness);
	vec3 diffuse = kd * albedo / PI;

	// Compute outgoing radiance: BRDF * incoming radiance * NdotL
	// light.diffuse carries the point light's radiance (colour * intensity).
	vec3 radiance = (diffuse + specular) * light.diffuse * NdotL;

	// Compute ambient color and apply ambient occlusion
	vec3 ambient = light.ambient * albedo * ao;

	color = ambient + radiance;
}
