#pragma once

#include <string>

#include "../utility/ray.h"
#include "../utility/vec3.h"
#include "../utility/color.h"

class skybox{
public:
    skybox(std::string skyboxFile) : skyboxFile(skyboxFile) {}

    void get_spherical_to_uv(const vec3& pt, double& u, double& v) const;

    color get_texture_sky_color(const ray& r) const;

private:
    std::string skyboxFile;
};