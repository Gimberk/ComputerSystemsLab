#pragma once

#include <memory>

class primitive;

struct hit_record{
	double t;
	std::shared_ptr<primitive> object;
	bool valid;

	hit_record(double t, std::shared_ptr<primitive> object) : t(t), object(object), valid(true) {}
	hit_record() : t(std::numeric_limits<double>::quiet_NaN()), valid(false) {}
};