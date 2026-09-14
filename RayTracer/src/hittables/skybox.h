#pragma once

#include <cmath>

#include "../utility/ray.h"
#include "../utility/vec3.h"
#include "../utility/color.h"
#include "../utility/random.h"

class skybox{
public:
    void get_spherical_to_uv(const vec3& pt, double& u, double& v) const {
        // use some fun trigonometry to find these
        double theta = std::acos(-pt.y);
        double phi = std::atan2(-pt.z, pt.x) + utility::PI;

        u = phi / (2.0 * utility::PI);
        v = theta / utility::PI;
    }

    color get_texture_sky_color(const ray& r) const {
        const vec3 unit_direction = unit_vector(r.direction());

        double u, v;
        get_spherical_to_uv(unit_direction, u, v);

        int x = static_cast<int>(u * width), y = static_cast<int>(v * height);

        x = clamp(x, 0, width - 1);
        y = clamp(y, 0, height - 1);
    }
};