#include "box.h"

const hit_record box::intersect(const ray& r) const {
	double closest_t = std::numeric_limits<double>::infinity();
	hit_record closest_hit;

	for (const plane& face : faces) {

	}
}

const point3 box::sample_random_point() const {

}

const vec3 box::get_normal(const point3& hit_point) const {
	
}