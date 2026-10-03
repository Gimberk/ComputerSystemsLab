#include "plane.h"

#include "../utility/random.h"

const hit_record plane::intersect(const ray& r) const {
	constexpr double epsilon = 1e-7;

	const double denominator = dot(normal, r.direction());

	if (std::abs(denominator) < 1e-8) return hit_record();

	const double t = dot(vertex - r.origin(), normal) / denominator;

	const point3 hit_point = r.point(t);
	const vec3 planar_vector = hit_point - vertex;

	const double proj_u = dot(planar_vector, u) / dot(u, u);
	const double proj_v = dot(planar_vector, v) / dot(v, v);

	if (std::abs(proj_u) > 1.0 || std::abs(proj_v) > 1.0) return hit_record();

	return t > epsilon ? hit_record(t, this) : hit_record();
}

const point3 plane::sample_random_point() const {
	const double sx = utility::random_double64() * 2 + 1;
	const double sy = utility::random_double64() * 2 + 1;

	return vertex + (sx * u) + (sy * v);
}