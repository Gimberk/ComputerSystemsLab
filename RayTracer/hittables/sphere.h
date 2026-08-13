#pragma once

#include <memory>

#include "../utility/vec3.h"
#include "Primitive.h"
#include "../utility/color.h"

class sphere : public primitive {
public:
	point3 center;
	double radius;

	sphere(const vec3 center, const double radius) : primitive(PrimitiveType::sphere), center(center), radius(radius) {}

	const hit_record intersect(const ray& r) const override;

	const vec3 get_normal(const vec3& hit_point) const override;
};