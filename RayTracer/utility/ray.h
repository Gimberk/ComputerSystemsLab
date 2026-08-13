#pragma once

#include "vec3.h"

class ray {
public:
	ray() {}

	ray(const point3& origin, const vec3& direction) : org(origin), dir(direction) {}

	const point3& origin() const { return org; }
	const vec3& direction() const { return dir; }

	point3 point(double time) const {
		return org + time * dir;
	}

private:
	point3 org;
	vec3 dir;
};