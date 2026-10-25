#pragma once

#include "../utility/vec3.h"
#include "../utility/ray.h"

class aabb {
public:
	point3 min, max;
	
	aabb() : min(point3(0, 0, 0)), max(point3(0, 0, 0)) {}

	aabb(const point3& min, const point3& max) : min(min), max(max) {}

	const bool intersect(const ray& r) const;

	void pad(const double padding);

	void expand(const aabb& other);

	void expand(const point3& pt);

	const int longest_axis() const;
};