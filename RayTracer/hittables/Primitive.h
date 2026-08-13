#pragma once

#include "../utility/ray.h"
#include "../utility/hit_record.h"

enum class PrimitiveType {
	sphere, triangle
};

class primitive {
public:
	PrimitiveType type;
	
	primitive(PrimitiveType type) : type(type) {}

	// returns a hit_record containing valid=true on hit and valid=false if no hit
	const virtual hit_record intersect(const ray& r) const = 0;
	
	const virtual vec3 get_normal(const point3& hit_point) const = 0;
};