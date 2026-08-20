#include "color.h"

void write_color_to_ppm(std::ostream& out, const color& pixel_color) {
	 const auto r = pixel_color.x;
	 const auto g = pixel_color.y;
	 const auto b = pixel_color.z;

	 // convert from [0,1] to [0,255]
	 const int ir = int(255.999 * r);
	 const int ig = int(255.999 * g);
	 const int ib = int(255.999 * b);

	 out << ir << ' ' << ig << ' ' << ib << '\n';
}