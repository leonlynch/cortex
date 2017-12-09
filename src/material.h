/**
 * @file material.h
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#ifndef CORTEX_MATERIAL
#define CORTEX_MATERIAL

#include <cmath>

class Material
{
public:
	enum class ShadingMode {
		None,
		Gouraud,
		BlinnPhong,
	};

	struct color3_t {
		float r;
		float g;
		float b;

		color3_t(float r, float g, float b);

		bool isBlack() const
		{
			static const float epsilon = 10e-3f;
			return std::fabs(r) < epsilon && std::fabs(g) < epsilon && std::fabs(b) < epsilon;
		}
	};

public:
	bool twosided;
	ShadingMode shading_mode;
	bool wireframe;

	color3_t ambient;
	color3_t diffuse;
	color3_t specular;
	float shininess;

public:
	Material();
	virtual ~Material();

	Material(const Material&) = delete;
	Material& operator=(const Material&) = delete;

	void setDefaultShadingMode();
};

#endif
