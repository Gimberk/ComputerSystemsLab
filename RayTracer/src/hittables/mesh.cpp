#include "mesh.h"
#include "triangle.h"

#include "../utility/random.h"

#include <iostream>
#include <cstdlib>

const hit_record mesh::intersect(const ray& ray) {
    triangle* closest = nullptr;
    float closest_t = std::numeric_limits<float>::max();
    int index = -1;

    for (int i = 0; i < tris.size(); i++) {
        std::shared_ptr<triangle>& tri = tris[i];
        const hit_record record = tri->intersect(ray);
        if (record.t > 0.0f && record.t < closest_t) {
            closest_t = record.t;
            index = i;
            closest = tri.get();
        }
    }

    if (closest_t == std::numeric_limits<float>::max()) return hit_record();
    return hit_record(closest_t, closest);
}

const vec3 mesh::get_normal(const point3& p) const {
    constexpr double EPSILON = 1e-6;

    for (const auto& tri : tris) {
        const point3& a = tri->points[0];
        const point3& b = tri->points[1];
        const point3& c = tri->points[2];

        const vec3 ab = b - a;
        const vec3 ac = c - a;
        const vec3 ap = p - a;

        const vec3 n = cross(ab, ac);
        const double n_len = n.length();

        if (n_len < EPSILON) {
            continue;
        }

        if (std::abs(dot(ap, n)) > EPSILON * n_len) {
            continue;
        }

        const double d00 = dot(ab, ab);
        const double d01 = dot(ab, ac);
        const double d11 = dot(ac, ac);
        const double d20 = dot(ap, ab);
        const double d21 = dot(ap, ac);

        const double denominator = d00 * d11 - d01 * d01;

        if (std::abs(denominator) < 1e-12) {
            continue;
        }

        const double v = (d11 * d20 - d01 * d21) / denominator;
        const double w = (d00 * d21 - d01 * d20) / denominator;
        const double u = 1.0 - v - w;

        if (u >= -EPSILON && v >= -EPSILON && w >= -EPSILON) {
            return n / n_len;
        }
    }

    std::cerr << "You're screwed. No such triangle exists. You mind doing better next time?\n";
    std::abort();
}

const point3 mesh::sample_random_point() const {
    const double r = utility::random_double64();
    const auto it = std::upper_bound(cdf.begin(), cdf.end(), r);
    const int tri_index = std::distance(cdf.begin(), it);

    return tris[tri_index]->sample_random_point();
}