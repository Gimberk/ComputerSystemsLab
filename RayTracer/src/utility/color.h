#pragma once

#include "vec3.h"

#include <iostream>

using color = vec3;

void write_color_to_ppm(std::ostream& out, const color& pixel_color);

// apply the ACES filter to curve bright values more realistically: https://en.wikipedia.org/wiki/Tone_mapping
static color aces_filmic(const color& col) {
    double a = 2.51;
    double b = 0.03;
    double c = 2.43;
    double d = 0.59;
    double e = 0.14;

    color result;
    result.x = (col.x * (a * col.x + b)) / (col.x * (c * col.x + d) + e);
    result.y = (col.y * (a * col.y + b)) / (col.y * (c * col.y + d) + e);
    result.z = (col.z * (a * col.z + b)) / (col.z * (c * col.z + d) + e);

    // explicitly clamp between 0 and 1 to fully prevent color overflow
    result.x = std::max(0.0, std::min(1.0, result.x));
    result.y = std::max(0.0, std::min(1.0, result.y));
    result.z = std::max(0.0, std::min(1.0, result.z));

    return result;
}