#pragma once

#include <algorithm>
#include <atomic>
#include <thread>
#include <vector>

#include "utility/vec3.h"
#include "utility/color.h"

#include "utility/ray.h"

#include "thread"

class camera {
public:
	// a bunch of lame camera math stuff
	const double aspect_ratio = 16.0 / 9.0;
	const int image_width = 10; // 600 / height = 1.77; height = 400 / 1.77; height = image_width / aspect_ratio 

	double focal_length = 1;
	point3 center;

	const int rays_per_pixel = 1;

	unsigned int max_threads = std::min(std::thread::hardware_concurrency(), (unsigned int)5);
	std::vector<std::thread> threads;

	double pitch = 0, yaw = 180;

	double speed = 0.1;

	const int image_height = int(image_width / aspect_ratio) < 1 ? 1 : image_width / aspect_ratio; // ensure the height is at least 1

	const double viewport_height = 2;
	const double viewport_width = viewport_height * (double(image_width) / image_height);

	const vec3 viewport_u = vec3(viewport_width, 0, 0), viewport_v = vec3(0, -viewport_height, 0);

	vec3 pixel_delta_u = viewport_u / image_width;
	vec3 pixel_delta_v = viewport_v / image_height;

	vec3 viewport_uppper_left = center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
	vec3 pixel00_location = viewport_uppper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

	camera() : center(vec3(0, 0, 0)), accumulation(image_width * image_height), accumulation_count(0) {}

	const vec3 forward_vector() const {
		const double pitch_rad = to_radians(pitch), yaw_rad = to_radians(yaw);
		return vec3(std::cos(pitch_rad) * std::sin(yaw_rad), sin(pitch_rad), cos(pitch_rad) * cos(yaw_rad));
	}

	const vec3 right_vector() const { 
		const double yaw_rad = to_radians(yaw);
		return vec3(std::cos(yaw_rad), 0, -std::sin(yaw_rad));
	}

	void append_accumulation(const int x, const int y, const color col);

	const std::vector<color>& get_final_accumulation() const { return accumulation; }

	void clear_accumulation();

	const std::atomic<int>& get_accumulation_count() const { return accumulation_count; }

	void increment_accumulation_count() { accumulation_count++; }

	void update_camera()
	{
		const vec3 vup(0, 1, 0);

		const vec3 w = -unit_vector(forward_vector());
		const vec3 u = unit_vector(cross(vup, w));
		const vec3 v = cross(w, u);

		const vec3 viewport_u = viewport_width * u;
		const vec3 viewport_v = viewport_height * -v;

		pixel_delta_u = viewport_u / image_width;
		pixel_delta_v = viewport_v / image_height;

		const vec3 viewport_upper_left =
			center
			- w * focal_length
			- viewport_u / 2
			- viewport_v / 2;

		const vec3 new_pixel00_location =
			viewport_upper_left
			+ 0.5 * pixel_delta_u
			+ 0.5 * pixel_delta_v;

		if (new_pixel00_location != pixel00_location)
			clear_accumulation();

		pixel00_location = new_pixel00_location;
	}

private:
	std::vector<color> accumulation;
	std::atomic<int> accumulation_count;
};