#pragma once

#include <memory>
#include <vector>

#include "Primitive.h"
#include "triangle.h"

class mesh : public primitive {
public:
	mesh(const std::vector<std::shared_ptr<triangle>>& tris) : primitive(primitive_type::mesh), tris(tris) {
		std::cout << "Created mesh with " << tris.size() << " triangles!" << '\n';
	}

	const hit_record intersect(const ray& ray) const override;

	const vec3 get_normal(const point3& hit_point) const override;

	const double get_surface_area() const override;
	
	const point3 sample_random_point() const override;

private:
	std::vector<std::shared_ptr<triangle>> tris;
};