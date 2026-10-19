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

	triangle(const std::array<vec3, 3>& points, std::unique_ptr<material> mat)
		: primitive(primitive_type::sphere, std::move(mat)), points(points) {
		const vec3 edge1 = points[0] - points[1], edge2 = points[2] - points[1];
		surface_area = 0.5 * cross(edge1, edge2).length();
	}

	const hit_record intersect(const ray& r) override;

	const point3 sample_random_point() const override;

	const double get_surface_area() const override { return surface_area; };

	const vec3 get_normal(const point3& hit_point) const override;

private:
	double surface_area;
};