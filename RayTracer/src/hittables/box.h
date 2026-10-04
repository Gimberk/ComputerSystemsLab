#pragma once

#include "Primitive.h"
#include "plane.h"

#include <memory>
#include <vector>

class box : public primitive {
public:
	point3 vertex;

	// height, length, depth
	vec3 dimensions;
	vec3 rotation;

	box(const box&) = delete;
	box& operator=(const box&) = delete;

	box(box&&) noexcept = default;
	box& operator=(box&&) noexcept = default;

	box(const point3& vertex, const vec3& normal, const double length, const double height,
		const double depth, const vec3& rotation, std::unique_ptr<material> mat)
		: primitive(primitive_type::box, std::move(mat)), vertex(vertex), dimensions(height, length, depth), rotation(rotation) {
        const double w = dimensions.x;
        const double h = dimensions.y;
        const double d = dimensions.z;

        plane left(vertex, vec3(-1, 0, 0), d, h);

        plane right(vertex + vec3(w, 0, 0), vec3(1, 0, 0), d, h);

        plane bottom(vertex, vec3(0, -1, 0), w, d);

        plane top(vertex + vec3(0, h, 0), vec3(0, 1, 0), w, d);

        plane front(vertex, vec3(0, 0, -1), w, h);

        plane back(vertex + vec3(0, 0, d), vec3(0, 0, 1), w, h);
	}

	const hit_record intersect(const ray& r) const override;

	const point3 sample_random_point() const override;

    const vec3 get_normal(const point3& hit_point) const override;
private:
	std::vector<plane> faces;
};