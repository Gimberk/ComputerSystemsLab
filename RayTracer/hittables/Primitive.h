#pragma once

#include "../utility/ray.h"
#include "../utility/hit_record.h"

#include "material.h"

enum class PrimitiveType {
	sphere, triangle
};

class primitive {
public:
	PrimitiveType type;
	std::unique_ptr<material> mat;

	primitive(primitive&&) noexcept = default;
	primitive& operator=(primitive&&) noexcept = default;

	primitive(PrimitiveType type) : type(type), mat(nullptr) {}
	primitive(PrimitiveType type, std::unique_ptr<material> mat) : type(type), mat(std::move(mat)) {}

	// returns a hit_record containing valid=true on hit and valid=false if no hit
	const virtual hit_record intersect(const ray& r) const = 0;
	
	const virtual vec3 get_normal(const point3& hit_point) const = 0;

	void set_material(std::unique_ptr<material> mat) { this->mat = std::move(mat); }

	bool has_material() { return bool(mat); }
};