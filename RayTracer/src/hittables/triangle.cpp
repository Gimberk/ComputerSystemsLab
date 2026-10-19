#include "triangle.h"

#include "../utility/random.h"

// we'll use the Moller-Trumbore algorithm--it's apparently very fast:
// https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
const hit_record triangle::intersect(const ray& r) {
	constexpr double epsilon = 1e-7;

	const vec3 edge1 = points[1] - points[0], edge2 = points[2] - points[0];

	const vec3 h = cross(r.direction(), edge2);
	const double a = dot(edge1, h);

	if (a > -epsilon && a < epsilon) return hit_record(); // ray is parallel--no intersection.

	const double f = 1.0 / a;
	const vec3 s = r.origin() - points[0];

	const double u = f * dot(s, h);
	if (u < 0.0 || u > 1.0) return hit_record(); // intersection point is outside edge1/2 bounds

	const vec3 q = cross(s, edge1);

	const double v = f * dot(r.direction(), q);
	if (v < 0.0 || (u + v) > 1.0) return hit_record(); // intersection point is outside remaining edges

	const double t = f * dot(edge2, q);

	return t > epsilon ? hit_record(t, this) : hit_record();
}

const vec3 triangle::get_normal(const point3& hit_point) const {
	const vec3 edge1 = points[1] - points[0], edge2 = points[2] - points[0];
	return cross(edge1, edge2);
}

const point3 triangle::sample_random_point() const {
	const double r1 = utility::random_double64(), r2 = utility::random_double64();
	const double sqrt_r1 = std::sqrt(r1);

	const double w_a = 1.0 - sqrt_r1;
	const double w_b = sqrt_r1 * (1.0 - r2);
	const double w_c = r2 * sqrt_r1;

	const point3 a = points[0], b = points[1], c = points[2];

	const double x = w_a * a.x + w_b * b.x + w_c * c.x;
	const double y = w_a * a.y + w_b * b.y + w_c * c.y;
	const double z = w_a * a.z + w_b * b.z + w_c * c.z;

	return point3(x, y, z);
};