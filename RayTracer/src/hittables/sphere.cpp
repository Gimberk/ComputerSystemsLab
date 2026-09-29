#include "sphere.h"

#include "../utility/random.h"

const hit_record sphere::intersect(const ray& r) const {
	const point3 O = r.origin(), C = center;
	const auto a = r.direction().length_squared();
	const auto h = dot(r.direction(), C - O);
	const auto c = (C - O).length_squared() - radius * radius;
	
	const auto discr = h * h - a * c;

	if (discr < 0) return hit_record();

	const double sqrt_discr = std::sqrt(discr);

	double t = (h - sqrt_discr) / a;

	if (t > 0.001) return hit_record(t, this);

	t = (h + sqrt_discr) / a;

	if (t > 0.001) return hit_record(t, this);

	return hit_record();
}

const point3 sphere::sample_random_point() const {
	const vec3 direction = utility::random_unit_vector();

	// y = C + RD
	return center + radius * direction;
}

const vec3 sphere::get_normal(const point3& hit_point) const {
	return unit_vector(hit_point - center);
}