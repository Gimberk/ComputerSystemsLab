#include "mesh.h"

#include "../utility/random.h"

#include <cstdlib>

const hit_record mesh::intersect(const ray& r) {
    return acceleration.intersect(r);
}

const vec3 mesh::get_normal(const point3& p) const {
    std::abort();
    /*constexpr double EPSILON = 1e-6;

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

    std::cerr << "You're screwed. No such triangle exists. Do you mind doing better next time?\n";
    std::abort();*/
}

const vec3 mesh::get_normal(const hit_record& record, const point3& hit_point) const {
    return record.object->get_normal(hit_point);
}

const point3 mesh::sample_random_point() const {
    const double r = utility::random_double64();
    const auto it = std::upper_bound(cdf.begin(), cdf.end(), r);
    const int tri_index = std::distance(cdf.begin(), it);

    return tris[tri_index]->sample_random_point();
}

const aabb mesh::compute_bounds() const {
    constexpr double inf = std::numeric_limits<double>::infinity();

    point3 min(inf, inf, inf), max(-inf, -inf, -inf);

    for (const auto& tri : tris) {
        for (const auto& p : tri->points) {
            min.x = std::min(min.x, p.x);
            min.y = std::min(min.y, p.y);
            min.z = std::min(min.z, p.z);

            max.x = std::max(max.x, p.x);
            max.y = std::max(max.y, p.y);
            max.z = std::max(max.z, p.z);
        }
    }

    if (tris.empty()) return aabb();

    aabb bb(min, max);
    
    constexpr double padding = 1e-6;
    bb.pad(padding);

    return bb;
}