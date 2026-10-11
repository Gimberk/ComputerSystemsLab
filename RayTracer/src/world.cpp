#include "world.h"

#include "hittables/skybox.h"

#include "utility/math.h"

#include <chrono>
#include <algorithm>
#include <future>

const color world::BLACK(0, 0, 0);
const color world::WHITE(1, 1, 1);
const color world::RED(1, 0, 0);
const color world::GREEN(0, 1, 0);
const color world::BLUE(0, 0, 1);

constexpr double epsilon = 1e-5;

const std::shared_ptr<primitive>& world::sample_random_light_source() const {
	return emissive_objects[utility::random_integer_bounds(0, emissive_objects.size() - 1)];
}

void world::create_object(const std::shared_ptr<primitive>& obj) {
	objects.emplace_back(obj);

	if (!obj->has_material()) return;

	if (obj->mat->emission_intensity > epsilon) emissive_objects.emplace_back(obj);
}

const bool world::find_any_hit(const ray & r, double max_t, primitive* ignore) const {
	for (const auto& obj : objects) {
		if (obj.get() == ignore) continue;

		if (!obj->mat->emission_color.near_zero()) continue;

		const hit_record hit = obj->intersect(r);
		if (hit.valid && hit.t > epsilon && hit.t < max_t) return true;
	}

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

color world::ray_color(const ray& r, const int depth, const double previous_bsdf_pdf, 
	const point3& previous_point, const bool has_previous_surface) const {
	ray_count++;
	
	if (depth >= max_ray_depth)
		return BLACK;

	hit_record record = intersect_world(r);

	if (!record.object)
		return world::BLACK;

	const primitive* obj = record.object;

	const color emission = obj->mat->emission_color * obj->mat->emission_intensity;
	if (!emission.near_zero()) {
		if (!has_previous_surface) return emission; // if this is a camera ray

		const double weight_bsdf = utility::power_heuristic(previous_bsdf_pdf, utility::light_pdf_for_hit(get_light_select_pdf(), 
															previous_point, r.origin(), *obj));
		return emission * weight_bsdf;
	}

	const point3 hit_point = r.point(record.t);
	const vec3 normal = obj->get_normal(hit_point);

	const color albedo = obj->mat->albedo;

	// this is the NEE implementation
	// if there are any occluders, we ignore any direct light
	const std::shared_ptr<primitive>& light_source = sample_random_light_source();

	const point3 y = light_source->sample_random_point(); // sample a point on the light
	const vec3 to_light = y - hit_point;
	const double dist = to_light.length();
	const double dist_squared = dist * dist;
	const vec3 light_direction = to_light / dist;

	const double surface_cosine = std::max(0.0, dot(normal, light_direction)); // the surface cosine

	color direct = world::BLACK;
	if (surface_cosine > 0.0) {
		// light's cosine
		const vec3 light_normal = light_source->get_normal(y);
		const double light_cosine = std::max(0.0, dot(light_normal, -light_direction));

		if (light_cosine > 0.0) {
			const ray shadow_ray(hit_point + normal * epsilon, light_direction);

			primitive* ignore = light_source.get();
			const bool blocked = find_any_hit(shadow_ray, dist * 1.01, ignore);

			if (!blocked) {
				// perform the rest of NEE now that there is a direct path
				const color Le = light_source->mat->emission_color * light_source->mat->emission_intensity;

				const color fr = albedo / utility::PI; // calculate the lambertian surface BRDF

				// PDF for the light source
				const double pdf_area = 1.0 / light_source->get_surface_area();
				const double pdf_select = get_light_select_pdf(); // the probability of selecting the light uniformly

				// solid-angle PDF
				const double light_pdf = pdf_select * pdf_area * dist_squared / light_cosine;

				// BSDF PDF for the same direction
				const double bsdf_pdf = surface_cosine / utility::PI;

				// finally, bring it all together for calculating the direct light contribution
				const double weight_light = utility::power_heuristic(light_pdf, bsdf_pdf);
				direct = Le * fr * surface_cosine * weight_light / light_pdf;
			}
		}
	}

	// compute the indirect light (BSDF)
	const vec3 wi = utility::random_cosine_direction(normal);
	const double pdf_bsdf = std::max(0.0, dot(normal, wi) / utility::PI);

	const ray scattered(hit_point + normal * epsilon, wi);

	const color indirect = albedo * ray_color(scattered, depth + 1, pdf_bsdf, hit_point, true);
	return direct + indirect;
}

const std::vector<unsigned char>* world::generate_image(thread_pool* pool, bool output) {
	ray_count = 0;
	auto start = std::chrono::high_resolution_clock::now();

	uint64_t frame_burn = cam->get_accumulation_count() * 12345;
	for (uint64_t i = 0; i < (frame_burn % 100); ++i) {
		utility::random_double64();
	}

	// for multithreading
	std::vector<std::future<void>> frame_futures;
	frame_futures.reserve(cam->max_threads);

	int rows_per_thread = cam->image_height  / cam->max_threads;
	
	// assign a number of rows to each available thread
	for (unsigned int i = 0; i < cam->max_threads; i++) {
		int start_y = i * rows_per_thread;
		int end_y = (i == cam->max_threads - 1) ? cam->image_height : start_y + rows_per_thread;

		frame_futures.emplace_back(pool->enqueue(&world::process_pixel_subsection, this, start_y, end_y));
	}

	// make sure each thread completes their rows before continuing.
	for (auto& future : frame_futures) future.wait();

	cam->increment_accumulation_count(); // only call once the frame is finished, not every time you update a pixel, dumbass. god damn.
										 // well that's rather mean

	/*
	if (!output){
		auto end = std::chrono::high_resolution_clock::now();
		time_for_last_frame = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() * 1e-9;

		return nullptr;
	}
	*/

	const std::vector<color>& accumulation = cam->get_final_accumulation();
	for (int y = 0; y < cam->image_height; y++) {
		for (int x = 0; x < cam->image_width; x++) {
			const int index = y * cam->image_width + x;
			//const int reverse_index = (cam->image_height - y - 1) * cam->image_width + x, 
			color averaged = accumulation[index] / cam->get_accumulation_count();

			// tone-map the pixels so they are all in the same format a monitor expects them to be.
			averaged = aces_filmic(averaged);

			// apply gamma correction
			const float inv_gamma = 1.0 / 2.2;
			float corrected_x = std::clamp(std::pow(static_cast<float>(averaged.x), inv_gamma), 0.0f, 1.0f);
			float corrected_y = std::clamp(std::pow(static_cast<float>(averaged.y), inv_gamma), 0.0f, 1.0f);
			float corrected_z = std::clamp(std::pow(static_cast<float>(averaged.z), inv_gamma), 0.0f, 1.0f);

			framebuffer[3 * index] = static_cast<unsigned char>(255.999 * corrected_x);
			framebuffer[3 * index + 1] = static_cast<unsigned char>(255.999 * corrected_y);
			framebuffer[3 * index + 2] = static_cast<unsigned char>(255.999 * corrected_z);
		}
	}

	auto end = std::chrono::high_resolution_clock::now();
	time_for_last_frame = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() * 1e-9;

	//std::cout << "Generated framebuffer in " << std::fixed << time_taken << std::setprecision(9) << " sec for " << ray_count << " rays." << '\n';
	return &framebuffer;
}

void world::process_pixel_subsection(int startY, int endY) {
	for (int row = startY; row < endY; row++){
		for (int c = 0; c < cam->image_width; c++){
			// broken code that is causing segfaults:
			auto pixel_center = cam->pixel00_location + cam->pixel_delta_u * c + cam->pixel_delta_v * row;
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
			cam->append_accumulation(c, row, average_color);
		}
	}
}