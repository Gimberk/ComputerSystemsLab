#pragma once

#include <string>

#include "../utility/ray.h"
#include "../utility/vec3.h"
#include "../utility/color.h"

class skybox{
public:
    skybox(std::string skyboxFile);
    skybox(std::string skyboxFile, double intensity_multiplier);

    void get_spherical_to_uv(const vec3& pt, double& u, double& v) const;

    color get_texture_sky_color(const ray& r) const;

    const bool skybox_is_valid() const { return valid; }

    skybox(const skybox&) = delete;
    skybox& operator=(const skybox&) = delete;

    // 2. Enable moving (transfers ownership of the pointer)
    skybox(skybox&& other) noexcept : data(other.data), intensity_multiplier(other.intensity_multiplier) {
        other.data = nullptr; // Clear the old pointer so it doesn't free it
    }

    ~skybox();
private:
    std::string skyboxFile;

    bool valid = true;

    double intensity_multiplier;

    float* data = nullptr;
    int width = 0, height = 0, channels = 0;
};