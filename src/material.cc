/**
 * @file material.cc
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include "material.h"

Material::color3_t::color3_t(float r, float g, float b)
: r(r),
  g(g),
  b(b)
{
}

Material::Material()
: twosided(false),
  shading_mode(ShadingMode::None),
  wireframe(false),
  ambient(0, 0, 0),
  diffuse(0, 0, 0),
  specular(0, 0, 0),
  shininess(0)
{
}

Material::~Material()
{
}

void Material::setDefaultShadingMode()
{
	static const float epsilon = 10e-3f;

	if (specular.isBlack() || std::fabs(shininess) < epsilon) {
		// no specular reflection, thus Gouraud shading
		shading_mode = ShadingMode::Gouraud;
	} else {
		// specular reflection, thus Blinn-Phong shading
		shading_mode = ShadingMode::BlinnPhong;
	}
}
