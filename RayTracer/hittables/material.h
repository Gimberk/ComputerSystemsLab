#pragma once

#include "../utility/color.h"

class material {
public:
	color albedo;

	material(color c) : albedo(c) {}
};

inline color get_null_mat() { return color(1, 0.294, 0.741); }