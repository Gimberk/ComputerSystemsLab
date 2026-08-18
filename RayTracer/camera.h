#pragma once

#include "utility/vec3.h"

class camera {
public:
	// a bunch of lame camera math stuff
	const double aspect_ratio = 16 / 9;
	const int image_width = 600; // 400 / height = 1.77; height = 400 / 1.77; height = image_width / aspect_ratio 

	double focal_length = 1;
	point3 center;

	const int rays_per_pixel = 16;

	const int image_height = int(image_width / aspect_ratio) < 1 ? 1 : image_width / aspect_ratio; // ensure the height is at least 1

	const double viewport_height = 2;
	const double viewport_width = viewport_height * (double(image_width) / image_height);

	const vec3 viewport_u = vec3(viewport_width, 0, 0), viewport_v = vec3(0, -viewport_height, 0);

	const vec3 pixel_delta_u = viewport_u / image_width;
	const vec3 pixel_delta_v = viewport_v / image_height;

	const vec3 viewport_uppper_left = center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
	const vec3 pixel00_location = viewport_uppper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

	camera() : center(vec3(0, 0, 0)) {}
};