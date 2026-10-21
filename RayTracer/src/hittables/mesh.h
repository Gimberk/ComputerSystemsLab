#pragma once

#include <memory>
#include <vector>

#include "Primitive.h"
#include "triangle.h"

#include <unordered_map>

class mesh : public primitive {
public:
	mesh(const std::vector<std::shared_ptr<triangle>>& tris) : primitive(primitive_type::mesh), tris(tris) {
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
	}

	const hit_record intersect(const ray& ray) override;

	const vec3 get_normal(const point3& hit_point) const override;

	const double get_surface_area() const override { return total_area; };
	
	const point3 sample_random_point() const override;

private:
	double total_area;

	std::vector<double> cdf; // cumulative distribution function

	std::vector<std::shared_ptr<triangle>> tris;
	std::unordered_map<point3, int, vec3_hash> hits; // hit, triangle index
};