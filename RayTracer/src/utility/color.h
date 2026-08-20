#pragma once

#include "vec3.h"

#include <iostream>

using color = vec3;

void write_color_to_ppm(std::ostream& out, const color& pixel_color);