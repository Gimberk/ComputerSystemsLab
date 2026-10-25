#include "aabb.h"

#include <algorithm>
#include <limits>

const bool aabb::intersect(const ray& r) const {
	double t_min = 0.0;
	double t_max = std::numeric_limits<double>::infinity();

	for (int axis = 0; axis < 3; axis++) {
		const double origin = component(r.origin(), axis);
		const double direction = component(r.direction(), axis);
		const double slab_min = component(min, axis);
		const double slab_max = component(max, axis);

		if (std::abs(direction) < 1e-12) {
			if (origin < slab_min || origin > slab_max) return false;
			continue;
		}

		double t0 = (slab_min - origin) / direction;
		double t1 = (slab_max - origin) / direction;

		if (t0 > t1) std::swap(t0, t1);

		t_min = std::max(t_min, t0);
		t_max = std::min(t_max, t1);

		if (t_max < t_min) return false;
	}

	return t_max > 0.0;
}

void aabb::pad(const double padding) {
	min.x -= padding;
	min.y -= padding;
	min.z -= padding;

	max.x += padding;
	max.y += padding;
	max.z += padding;
}

void aabb::expand(const aabb& other) {
	for (int axis = 0; axis < 3; axis++) {
		min[axis] = std::min(min[axis], other.min[axis]);
		max[axis] = std::max(max[axis], other.max[axis]);
	}
}

void aabb::expand(const point3& other) {
	for (int axis = 0; axis < 3; axis++) {
		min[axis] = std::min(min[axis], other[0]);
		max[axis] = std::max(max[axis], other[axis]);
	}
}

const int aabb::longest_axis() const {
	const point3 size = max - min;
	return std::max_element(&size[0], &size[3]) - &size[0];
}