#pragma once

#include <memory>
#include <vector>

#include "primitives/Primitive.h"
#include "primitives/triangle.h"

#include "aabb.h"

class mesh : public primitive {
public:
	mesh(const std::vector<std::shared_ptr<triangle>>& tris, std::unique_ptr<material> mat) : primitive(primitive_type::mesh, 
		std::move(mat)), tris(tris), bounding_box(compute_bounds()) {
		std::cout << "Created mesh with " << tris.size() << " triangles!" << '\n';

		for (const auto& tri : tris) total_area += tri->get_surface_area();

		std::cout << "Total surface area: " << total_area << '\n';

		cdf.resize(tris.size());

		double cum_sum = 0.0;
		for (int k = 0; k < cdf.size(); k++) {
			cum_sum += tris[k]->get_surface_area();
			cdf[k] = cum_sum / total_area;
		}

		std::cout << "Done\n";

		for (int i = 0; i < 10; i++) {
			const auto& p = tris[i]->points;

			std::cout << "Triangle " << i << ":\n";
			std::cout << "  A: " << p[0] << '\n';
			std::cout << "  B: " << p[1] << '\n';
			std::cout << "  C: " << p[2] << '\n';
			std::cout << "  normal: " << tris[i]->get_normal(vec3()) << '\n';
			std::cout << "  area: " << tris[i]->get_surface_area() << "\n\n";
		}

		std::cout << "Mesh bounds:\n";
		std::cout << "min = " << bounding_box.min << '\n';
		std::cout << "max = " << bounding_box.max << '\n';
	}

	const hit_record intersect(const ray& r) override;

	const vec3 get_normal(const point3& hit_point) const override;

	const vec3 get_normal(const hit_record& record, const point3& hit_point) const override;

	const double get_surface_area() const override { return total_area; };
	
	const point3 sample_random_point() const override;

	const aabb compute_bounds() const;

private:
	double total_area;

	std::vector<double> cdf; // cumulative distribution function

	std::vector<std::shared_ptr<triangle>> tris;
	//std::unordered_map<point3, int, vec3_hash> hits; // hit, triangle index

	const aabb bounding_box;
};