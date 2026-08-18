#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

#include "world.h"
#include "camera.h"

#include "utility/color.h"
#include "utility/ray.h"
#include "utility/random.h"

#include "hittables/sphere.h"

world scene;
camera cam;

static bool find_any_hit(const ray& r) {
	for (const std::shared_ptr<primitive>& obj : scene.get_objects()) if (obj->intersect(r).valid) return true;
}

static color ray_color(const ray& r, const int depth = 0) {
	if (depth > scene.max_ray_depth) return world::BLACK;

	hit_record record = scene.intersect_world(r);
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
	const ray shadow_ray(r.point(record.t), scene.sun_direction);
	if (!find_any_hit(shadow_ray)) direct_light = obj_color * scene.sun_color * std::max(0.0, dot(normal, scene.sun_direction));

	vec3 scatter_direction = normal + utility::random_unit_vector();
	if (scatter_direction.near_zero()) scatter_direction = normal;

	ray scattered(r.point(record.t), scatter_direction);
	return direct_light + obj_color * ray_color(scattered, depth + 1);
}

int main()
{
	scene.create_object(std::make_shared<sphere>(point3(0, 0, -1), 0.5, std::make_unique<material>(color(0.67, 0.5, 1))));
	scene.create_object(std::make_shared<sphere>(point3(0, -100.5, -1), 100, std::make_unique<material>(color(0.4, 0.95, 0.4))));

	std::ofstream out_file("img.ppm");
	
	if (!out_file) {
		std::cerr << "Error: Could not open \"img.ppm\" for writing!" << std::endl;
		return -1;
	}

	out_file << "P3\n"; // rgb ppm format
	out_file << cam.image_width << ' ' << cam.image_height << '\n';
	out_file << "255\n";

	for (int row = 0; row < cam.image_height; row++) {
		for (int col = 0; col < cam.image_width; col++) {
			auto pixel_center = cam.pixel00_location + cam.pixel_delta_u * col + cam.pixel_delta_v * row;
			color average_color(0, 0, 0);

			for (int i = 0; i < cam.rays_per_pixel; i++) {
				double offset_u = utility::random_double64() - 0.5;
				double offset_v = utility::random_double64() - 0.5;

				auto sample_point = pixel_center + (cam.pixel_delta_u * offset_u) + (cam.pixel_delta_v * offset_v);
				
				auto ray_direction = sample_point - cam.center;
				ray r(cam.center, ray_direction);

				average_color += ray_color(r);
			}

			average_color /= cam.rays_per_pixel;
			write_color_to_ppm(out_file, average_color);
		}
	}

	std::cout << "Done.";

	out_file.close();
	std::cin.get();
}