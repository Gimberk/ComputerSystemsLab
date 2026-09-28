#pragma once

#include "../utility/color.h"

class material {
public:
	color albedo;

	double metallic;
	double roughness;

	vec3 emission_color;
	double emission_intensity;

	material(color c) : 
		albedo(c), metallic(0), roughness(0), emission_intensity(0), emission_color() {}

	material(color c, double metallic, double roughness) : 
		albedo(c), metallic(metallic), roughness(roughness), emission_intensity(0), emission_color() {}
	material(color c, vec3 emission_color, double emission_intensity) : 
		albedo(c), emission_color(emission_color), emission_intensity(emission_intensity) {}
};

inline color get_null_mat() { return color(1, 0.294, 0.741); }