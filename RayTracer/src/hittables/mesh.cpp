#include "mesh.h"
#include "triangle.h"

const hit_record mesh::intersect(const ray& ray) const {
    triangle* closest;
    float closest_t = std::numeric_limits<float>::max();

    for (int i = 0; i < tris.size(); i++) {
        const std::shared_ptr<triangle> tri = tris[i];
        const hit_record record = tri->intersect(ray);
        if (record.t > 0.0f && record.t < closest_t) {
            closest_t = record.t;
            closest = tri.get();
        }
    }

    if (closest == nullptr) return hit_record();
    return hit_record(closest_t, closest);
}

const vec3 mesh::get_normal(const point3& hit_point) const {
    
}

const double mesh::get_surface_area() const {
    double sum = 0;
    for (const std::shared_ptr<triangle>& tri : tris) sum += tri->get_surface_area();
    return sum;
}
	
const point3 mesh::sample_random_point() const {

}