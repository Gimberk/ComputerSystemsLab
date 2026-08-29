#include "camera.h"

void camera::append_accumulation(const int x, const int y, const color col) {
	const int index = y * image_width + x;
	accumulation[index] += col;
}

void camera::clear_accumulation() {
	std::fill(accumulation.begin(), accumulation.end(), color(0, 0, 0));
	accumulation_count = 0;
}