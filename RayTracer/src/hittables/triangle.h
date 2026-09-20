#pragma once

#include <array>

#include "Primitive.h"

class triangle : public primitive
{
public:
	std::array<vec3, 3> points;

	triangle(const triangle&) = delete;
	triangle& operator=(const triangle&) = delete;

	triangle(triangle&&) noexcept = default;
	triangle& operator=(triangle&&) noexcept = default;

	triangle(const std::array<vec3, 3>& points) : primitive(primitive_type::triangle), points(points) {}

	triangle(const std::array<vec3, 3>& points, std::unique_ptr<material> mat)
		: primitive(primitive_type::sphere, std::move(mat)), points(points) {}

	const hit_record intersect(const ray& r) const override;

	const vec3 get_normal(const point3& hit_point) const override;
};