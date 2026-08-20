#pragma once

#include <memory>

#include "../utility/vec3.h"
#include "Primitive.h"
#include "../utility/color.h"

class sphere : public primitive {
public:
	point3 center;
	double radius;
	
	sphere(const sphere&) = delete;
	sphere& operator=(const sphere&) = delete;

	sphere(sphere&&) noexcept = default;
	sphere& operator=(sphere&&) noexcept = default;

	sphere(const point3& center, const double radius) : primitive(primitive_type::sphere), center(center), radius(radius) {}

	sphere(const point3& center, const double radius, std::unique_ptr<material> mat) 
		: primitive(primitive_type::sphere, std::move(mat)), center(center), radius(radius) {}

	const hit_record intersect(const ray& r) const override;

	const vec3 get_normal(const point3& hit_point) const override;
};