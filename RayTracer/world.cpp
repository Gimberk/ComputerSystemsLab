#include "world.h"

const color world::BLACK(0, 0, 0);
const color world::WHITE(1, 1, 1);
const color world::RED(1, 0, 0);
const color world::GREEN(0, 1, 0);
const color world::BLUE(0, 0, 1);

void world::create_object(const std::shared_ptr<primitive>& obj) {
	objects.emplace_back(obj);
}

const std::vector<std::shared_ptr<primitive>> world::get_objects() const { return objects; }

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