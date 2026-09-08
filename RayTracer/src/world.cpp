#include "world.h"

#include <chrono>

const color world::BLACK(0, 0, 0);
const color world::WHITE(1, 1, 1);
const color world::RED(1, 0, 0);
const color world::GREEN(0, 1, 0);
const color world::BLUE(0, 0, 1);

void world::create_object(const std::shared_ptr<primitive>& obj) {
	objects.emplace_back(obj);
}

const bool world::find_any_hit(const ray & r) const {
	for (const std::shared_ptr<primitive>& obj : get_objects()) if (obj->intersect(r).valid) return true;
	return false;
}

const std::vector<std::shared_ptr<primitive>>& world::get_objects() const { return objects; }

const hit_record world::intersect_world(const ray& r) const {
	double closest_so_far = std::numeric_limits<double>::infinity();
	const primitive* closest_obj = nullptr;

	for (const std::shared_ptr<primitive>& obj : get_objects()) {
		const hit_record hit = obj->intersect(r);

		if (hit.valid && hit.t < closest_so_far && hit.t > 0.001) {
			closest_so_far = hit.t;
			closest_obj = hit.object;
		}
	}

	if (!closest_obj) return hit_record();
	else return hit_record{ closest_so_far, closest_obj };
}

static std::atomic<uint16_t> ray_count = 0;

color world::ray_color(const ray& r, const int depth) const {
	ray_count++;
	if (depth > max_ray_depth) return world::BLACK;

	hit_record record = intersect_world(r);
	const primitive* closest_obj = record.object;

	if (!record.object) {
		// if no solution:
		// we blend from baby-blue to white
		vec3 unit_direction = unit_vector(r.direction());
		auto x = 0.5 * (unit_direction.y + 1.0);
		return (1.0 - x) * color(1, 1, 1) + x * color(0.5, 0.7, 1.0);
	}

	const color obj_color = closest_obj->has_material() ? closest_obj->mat->albedo : get_null_mat();
	const vec3 normal = closest_obj->get_normal(r.point(record.t));

	color direct_light = world::BLACK;
	const ray shadow_ray(r.point(record.t), sun_direction);
	if (!find_any_hit(shadow_ray)) direct_light = obj_color * sun_color * std::max(0.0, dot(normal, sun_direction));

	vec3 scatter_direction = normal + utility::random_unit_vector();
	if (scatter_direction.near_zero()) scatter_direction = normal;

	ray scattered(r.point(record.t), scatter_direction);
	return direct_light + obj_color * ray_color(scattered, depth + 1);
}

const std::vector<unsigned char>* world::generate_image(bool output) {
	ray_count = 0;
	auto start = std::chrono::high_resolution_clock::now();

	cam->threads.clear();

	int rows_per_thread = cam->image_height / cam->max_threads;
	std::cout << rows_per_thread << '\n';

	for (unsigned int i = 0; i < cam->max_threads; i++){
		int startY = i * rows_per_thread;
		int endY = (i == cam->max_threads - 1) ? cam->image_height : startY + rows_per_thread;

		std::cout << "For thread " << i << ": (" << startY << ", " << endY << ")\n";

		cam->threads.emplace_back(&world::process_pixel_subsection, this, startY, endY);
	}

	for (unsigned int i = 0; i < cam->max_threads; i++){
		if (cam->threads[i].joinable()) cam->threads[i].join();
	}

	cam->increment_accumulation_count(); // only call once the frame is finished, not every time you update a pixel dumbass. god damn.
										 // well that's rather mean 	

	if (!output){
		auto end = std::chrono::high_resolution_clock::now();
		time_for_last_frame = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() * 1e-9;

		return nullptr;
	}

	const std::vector<color>& accumulation = cam->get_final_accumulation();
	for (int y = 0; y < cam->image_height; y++) {
		for (int x = 0; x < cam->image_width; x++) {
			const int index = y * cam->image_width + x;
			//const int reverse_index = (cam->image_height - y - 1) * cam->image_width + x, 
			const color averaged = accumulation[index] / cam->get_accumulation_count();

			framebuffer[3 * index] = static_cast<unsigned char>(255.999 * averaged.x);
			framebuffer[3 * index + 1] = static_cast<unsigned char>(255.999 * averaged.y);
			framebuffer[3 * index + 2] = static_cast<unsigned char>(255.999 * averaged.z);
		}
	}

	auto end = std::chrono::high_resolution_clock::now();
	double time_for_last_frame = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() * 1e-9;

	//std::cout << "Generated framebuffer in " << std::fixed << time_taken << std::setprecision(9) << " sec for " << ray_count << " rays." << '\n';
	return &framebuffer;
}

void world::process_pixel_subsection(int startY, int endY) {
	std::cout << "(" << startY << ", " << endY << ") done.\n";
	for (int r = startY; r <= endY; r++){
		for (int c = 0; c < cam->image_width; c++){
			// broken code that is causing segfaults:
			/*
			auto pixel_center = cam->pixel00_location + cam->pixel_delta_u * c + cam->pixel_delta_v * r;
			color average_color(0, 0, 0); // averaging the randomness of ray reflections fixes the jagged edges

			for (int i = 0; i < cam->rays_per_pixel; i++) {
				double offset_u = utility::random_double64() - 0.5;
				double offset_v = utility::random_double64() - 0.5;

				auto sample_point = pixel_center + (cam->pixel_delta_u * offset_u) + (cam->pixel_delta_v * offset_v);

				auto ray_direction = sample_point - cam->center;
				ray r(cam->center, ray_direction);

				average_color += ray_color(r);
			}

			average_color /= cam->rays_per_pixel;

			// add this color to the accumulation of each frame so far
			cam->append_accumulation(c, r, average_color);
			*/
		}
	}
}