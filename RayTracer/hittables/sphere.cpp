#include "sphere.h"

const hit_record sphere::intersect(const ray& r) const {
	const point3 O = r.origin(), C = center;
	const auto a = r.direction().length_squared();
	const auto h = dot(r.direction(), C - O);
	const auto c = (C - O).length_squared() - radius * radius;
	const auto discr = h * h - a * c;

	if (discr < 0) return hit_record();

	const double t = (h - sqrt(discr)) / a;

	return hit_record(t, std::make_unique<sphere>(*this));
}

const vec3 sphere::get_normal(const vec3& hit_point) const {
	return unit_vector(hit_point - center);
}