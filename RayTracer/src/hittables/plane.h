#pragma once

#include "Primitive.h"

class plane : public primitive
{
public:
	point3 vertex;

	double width, height;

	plane(const plane&) = delete;
	plane& operator=(const plane&) = delete;

	plane(plane&&) noexcept = default;
	plane& operator=(plane&&) noexcept = default;

	// for use only with creating boxes.
	plane(const point3& vertex, const vec3& normal, const double width, const double height)
		: primitive(primitive_type::plane), vertex(vertex), normal(normal), width(width), height(height) {
		auxiliary = (std::abs(this->normal.x) > 0.9) ? vec3(0, 1, 0) : vec3(1, 0, 0);

		u_axis = unit_vector(cross(auxiliary, this->normal));
		v_axis = unit_vector(cross(this->normal, u_axis));

		u = u_axis * (width / 2.0);
		v = v_axis * (height / 2.0);
	}

	plane(const point3& vertex, const vec3& normal, const double width, const double height, std::unique_ptr<material> mat)
		: primitive(primitive_type::plane, std::move(mat)), vertex(vertex), normal(normal), width(width), height(height) {
		auxiliary = (std::abs(this->normal.x) > 0.9) ? vec3(0, 1, 0) : vec3(1, 0, 0);

		u_axis = unit_vector(cross(auxiliary, this->normal));
		v_axis = unit_vector(cross(this->normal, u_axis));

		u = u_axis * (width / 2.0);
		v = v_axis * (height / 2.0);
	}

	const hit_record intersect(const ray& r) const override;

	const point3 sample_random_point() const override;

	const vec3 get_normal(const point3& hit_point) const override { return normal; }

private:
	vec3 normal;

	vec3 auxiliary;
	vec3 u_axis, v_axis;
	vec3 u, v;
};