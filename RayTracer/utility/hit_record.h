#pragma once

#include <memory>

class primitive;

struct hit_record{
	const double t;
	const primitive* object;
	const bool valid;

	hit_record(const double t, const primitive* object) : t(t), object(object), valid(true) {}
	hit_record() : t(std::numeric_limits<double>::quiet_NaN()), valid(false), object(nullptr) {}
};