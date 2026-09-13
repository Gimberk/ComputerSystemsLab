#pragma once

#include "../utility/color.h"

class material {
public:
	color albedo;

	double metallic;
	double roughness;

	material(color c) : albedo(c), metallic(0), roughness(0) {}

	material(color c, double metallic, double roughness) : albedo(c), metallic(metallic), roughness(roughness) {}
};

inline color get_null_mat() { return color(1, 0.294, 0.741); }