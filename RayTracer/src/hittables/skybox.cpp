#include "skybox.h"

#include "../utility/random.h"

#include "../ext/stb_image.h"

#include <algorithm>
#include <cmath>

skybox::skybox(std::string skyboxFile) : skyboxFile(skyboxFile), intensity_multiplier(1) {
    // load data once in constructor, not every query bro.
    data = stbi_loadf(skyboxFile.data(), &width, &height, &channels, 3);

    if (!data){
        valid = false;
        std::cerr << "Failed to load HDR image: " << skyboxFile << "\nReason: " << stbi_failure_reason() << '\n' << std::endl;
    }
}

skybox::skybox(std::string skyboxFile, double intensity_multiplier) : skyboxFile(skyboxFile), intensity_multiplier(intensity_multiplier) {
    // load data once in constructor, not every query bro.
    data = stbi_loadf(skyboxFile.data(), &width, &height, &channels, 3);

    if (!data){
        valid = false;
        std::cerr << "Failed to load HDR image: " << skyboxFile << "\nReason: " << stbi_failure_reason() << '\n' << std::endl;
    }
}

skybox::~skybox() { stbi_image_free(data); }

void skybox::get_spherical_to_uv(const vec3& pt, double& u, double& v) const {
    // use some fun trigonometry to find these
    double theta = std::acos(-pt.y);
    double phi = std::atan2(-pt.z, pt.x) + utility::PI;

    u = phi / (2.0 * utility::PI);
    v = theta / utility::PI;
}

color skybox::get_texture_sky_color(const ray& r) const {
    const vec3 unit_direction = unit_vector(r.direction());

    double u, v;
    get_spherical_to_uv(unit_direction, u, v);

    int x = static_cast<int>(u * width), y = static_cast<int>(v * height);

    x = std::clamp(x, 0, width - 1);
    y = std::clamp(y, 0, height - 1);

    y = height - y;

    const int index = 3 * (y * width + x);

    color out_color(data[index], data[index + 1], data[index + 2]);
    out_color *= intensity_multiplier;

    out_color = aces_filmic(out_color);

    return out_color;
}