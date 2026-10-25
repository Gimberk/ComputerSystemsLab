#pragma once

#include "random.h"
#include "vec3.h"

#include "../hittables/primitives/Primitive.h"

namespace utility {
	inline double light_pdf_for_hit(const double light_pdf_select, const point3& previous_point, 
		const point3& light_hit, const primitive& light) {
		const vec3 to_light = light_hit - previous_point;
		const double dist_squared = to_light.length_squared();
		const double dist = std::sqrt(dist_squared);
		const vec3 direction = to_light / dist; // create a unit_vector for dir

		const vec3 light_normal = light.get_normal(light_hit);
		const double light_cosine = std::abs(dot(light_normal, -direction));

		if (light_cosine <= 1e-8) return 0.0;

		const double pdf_area = 1.0 / light.get_surface_area();

		return light_pdf_select * pdf_area * dist_squared / light_cosine;
	}

	inline vec3 random_unit_vector() {
		const double u = random_double64(), v = random_double64();

		// generate a random spherical coordinate
		const double z = 1.0 - 2.0 * u;
		const double r = std::sqrt(std::max(0.0, 1.0 - z * z));
		const double theta = 2.0 * PI * v;

		return vec3(r * std::cos(theta), r * std::sin(theta), z);
	}

	inline vec3 random_cosine_direction(const vec3& normal) {
		const double r1 = random_double64(), r2 = random_double64();

		const double theta = 2.0 * PI * r1;

		const double z = sqrt(1.0 - r2);
		const double radius = sqrt(r2);

		const double x = radius * cos(theta);
		const double y = radius * sin(theta);

		const vec3 w = normal;

		const vec3 a = (std::abs(w.x) > 0.9) ? vec3(0, 1, 0) : vec3(1, 0, 0);

		const vec3 u = unit_vector(cross(a, w));
		const vec3 v = cross(u, w);

		return unit_vector(u * x + v * y + w * z);
	}

	/*
		chosen MIS weighting strategy: https://64.github.io/multiple-importance-sampling/
		this provides more weight to the sample that was more likely to generate (i.e. either direct or indirect lighting)
	*/
	inline double power_heuristic(const double pdf_a, const double pdf_b) {
		const double a = pdf_a * pdf_a, b = pdf_b * pdf_b;
		const double sum = a + b;

		return sum > 0.0 ? a / sum : 0.0;
	}
}