#include "triangle.h"

// we'll use the Moller-Trumbore algorithm--it's apparently very fast:
// https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
const hit_record triangle::intersect(const ray& r) const {
	const double epsilon = 0.0000001;

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

	if (t > epsilon) return hit_record(t, this);
	else return hit_record(); // intersection is behind camera
}

const vec3 triangle::get_normal(const point3& hit_point) const {
	const vec3 edge1 = points[1] - points[0], edge2 = points[2] - points[0];
	return cross(edge1, edge2);
}