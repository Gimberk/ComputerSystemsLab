#pragma once

#include <memory>
#include <vector>

#include "hittables/Primitive.h"

#include "utility/hit_record.h"
#include "utility/color.h"
#include "utility/vec3.h"

class world {
public:
	static const color BLACK;
	static const color WHITE;
	static const color RED;
	static const color GREEN;
	static const color BLUE;

	color sun_color;
	vec3 sun_direction;

	int max_ray_depth;

	world() : sun_color(color(254.0 / 255.0, 211.0 / 255.0, 60.0 / 255.0)), sun_direction(vec3(0.707, 0.707, 0)), max_ray_depth(16) {}

	world(int max_ray_depth, color sun_color, vec3 sun_direction) : sun_color(sun_color), sun_direction(sun_direction), max_ray_depth(max_ray_depth) {}

	void create_object(const std::shared_ptr<primitive>&);

	const std::vector<std::shared_ptr<primitive>> get_objects() const;

	const hit_record intersect_world(const ray& r) const;

private:
	std::vector<std::shared_ptr<primitive>> objects;
};