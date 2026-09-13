#pragma once

#include "../utility/color.h"

class material {
public:
	color albedo;

	double metallic;
	double roughness;

	material(color c) : albedo(c), metallic(0), roughness(0) {}
};

inline color get_null_mat() { return color(1, 0.294, 0.741); }