#pragma once

#include "../utility/ray.h"
#include "../utility/hit_record.h"

#include "material.h"

enum class primitive_type {
	sphere, triangle
};

class primitive {
public:
	primitive_type type;
	std::unique_ptr<material> mat;

	primitive(primitive&&) noexcept = default;
	primitive& operator=(primitive&&) noexcept = default;

	primitive(primitive_type type) : type(type), mat(nullptr) {}
	primitive(primitive_type type, std::unique_ptr<material> mat) : type(type), mat(std::move(mat)) {}

	// returns a hit_record containing valid=true on hit and valid=false if no hit
	const virtual hit_record intersect(const ray& r) const = 0;
	
	const virtual vec3 get_normal(const point3& hit_point) const = 0;

	void set_material(std::unique_ptr<material> mat) { this->mat = std::move(mat); }

	const bool has_material() const { return bool(mat); }
};