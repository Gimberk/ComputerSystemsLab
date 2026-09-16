#include "skybox.h"

#include "../utility/random.h"

#include "../ext/stb_image.h"

#include <algorithm>
#include <cmath>

void skybox::get_spherical_to_uv(const vec3& pt, double& u, double& v) const {
    // use some fun trigonometry to find these
    double theta = std::acos(-pt.y);
    double phi = std::atan2(-pt.z, pt.x) + utility::PI;

    u = phi / (2.0 * utility::PI);
    v = theta / utility::PI;
}

color skybox::get_texture_sky_color(const ray& r) const {
    int width, height, channels;

    float* data = stbi_loadf(skyboxFile.data(), &width, &height, &channels, 3);

    if (!data){
        std::cerr << "Failed to load HDR image: " << skyboxFile << "\nReason: " << stbi_failure_reason() << std::endl;
        return color(0,0,0);
    }

    const vec3 unit_direction = unit_vector(r.direction());
    double u, v;
    get_spherical_to_uv(unit_direction, u, v);

    int x = static_cast<int>(u * width), y = static_cast<int>(v * height);

    x = std::clamp(x, 0, width - 1);
    y = std::clamp(y, 0, height - 1);

    const int index = 4 * (y * width + x);
    const float r_val = data[index], g_val = data[index + 1], b_val = data[index + 2];

    stbi_image_free(data);

    return color(r_val, g_val, b_val);
}