#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "utility/vec3.h"
#include "utility/color.h"
#include "utility/ray.h"
#include "utility/random.h"
#include "utility/hit_record.h"

#include "hittables/Primitive.h"
#include "hittables/sphere.h"

std::vector<std::shared_ptr<primitive>> objects;

static color ray_color(const ray& r) {
	bool hit_anything = false;
	double closest_so_far = std::numeric_limits<double>::infinity();
	std::shared_ptr<primitive> closest_obj = nullptr;
	int closest_index = 0;

	int i = 0;
	for (const std::shared_ptr<primitive>& obj : objects) {
		const hit_record hit = obj->intersect(r);

		if (hit.valid && hit.t < closest_so_far) {
			hit_anything = true;
			closest_so_far = hit.t;
			closest_obj = obj;
			closest_index = i;
		}

		i++;
	}

	if (hit_anything) {
		return closest_index == 0 ? color(0, 0, 0) : color(1, 1, 1);
	}

	// if no solution:
	// we blend from baby-blue to white
	vec3 unit_direction = unit_vector(r.direction());
	auto x = 0.5 * (unit_direction.y + 1.0);
	return (1.0 - x) * color(1, 1, 1) + x * color(0.5, 0.7, 1.0);
}

int main()
{
	objects.push_back(std::make_shared<sphere>(point3(0, 0, -2), 0.75));
	objects.push_back(std::make_shared<sphere>(point3(0, 0, -1), 0.25));

	// a bunch of lame camera math stuff
	const double aspect_ratio = 16 / 9;
	const int image_width = 400; // 400 / height = 1.77; height = 400 / 1.77; height = image_width / aspect_ratio 
	
	double focal_length = 1;
	point3 camera_center = point3(0, 0, 0);

	const int rays_per_pixel = 16;

	const int image_height = int(image_width / aspect_ratio) < 1 ? 1 : image_width / aspect_ratio; // ensure the height is at least 1
	
	const double viewport_height = 2;
	const double viewport_width = viewport_height * (double(image_width) / image_height);

	const vec3 viewport_u = vec3(viewport_width, 0, 0), viewport_v = vec3(0, -viewport_height, 0);

	const auto pixel_delta_u = viewport_u / image_width;
	const auto pixel_delta_v = viewport_v / image_height;

	const auto viewport_uppper_left = camera_center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
	const auto pixel00_location = viewport_uppper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

	std::ofstream out_file("img.ppm");
	
	if (!out_file) {
		std::cerr << "Error: Could not open \"img.ppm\" for writing!" << std::endl;
		return -1;
	}

	out_file << "P3\n"; // rgb ppm format
	out_file << image_width << ' ' << image_height << '\n';
	out_file << "255\n";


	const auto half_pixel_width = pixel_delta_u / 2;
	const auto half_pixel_height = pixel_delta_v / 2;
	for (int row = 0; row < image_height; row++) {
		for (int col = 0; col < image_width; col++) {
			auto pixel_center = pixel00_location + pixel_delta_u * col + pixel_delta_v * row;
			color average_color(0, 0, 0);

			for (int i = 0; i < rays_per_pixel; i++) {
				double offset_u = utility::random_double64() - 0.5;
				double offset_v = utility::random_double64() - 0.5;

				auto sample_point = pixel_center + (pixel_delta_u * offset_u) + (pixel_delta_v * offset_v);
				
				auto ray_direction = sample_point - camera_center;
				ray r(camera_center, ray_direction);

				average_color += ray_color(r);
			}

			average_color /= rays_per_pixel;
			write_color_to_ppm(out_file, average_color);
		}
	}

	std::cout << "Done.";

	out_file.close();
	std::cin.get();
}