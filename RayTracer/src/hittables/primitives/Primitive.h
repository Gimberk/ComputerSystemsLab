#pragma once

#include "../../utility/ray.h"
#include "../../utility/hit_record.h"

#include "../material.h"

enum class primitive_type {
	sphere, triangle, plane, mesh
};

class primitive {
public:
	primitive_type type;

	primitive(const primitive&) = delete;
	primitive& operator=(const primitive&) = delete;

	//primitive(primitive_type type) : type(type){}

	primitive(primitive_type type, std::unique_ptr<material> mat) : type(type), mat(std::move(mat)) {}

	// returns a hit_record containing valid=true on hit and valid=false if no hit
	const virtual hit_record intersect(const ray& r) = 0;
	
	const virtual vec3 get_normal(const point3& hit_point) const = 0;

	const virtual vec3 get_normal(const hit_record& record, const point3& hit_point) const { return get_normal(hit_point); }

	const virtual double get_surface_area() const = 0;
	
	const virtual point3 sample_random_point() const = 0;

	const bool has_material() const { return bool(mat); }

	virtual const material* get_material() const { return mat.get(); }

private:
	std::unique_ptr<material> mat;
};