#pragma once

#include "vec3.h"

#include <cstdint>
#include <numbers>
#include <cmath>
#include <random>

namespace utility {
	constexpr double PI = 3.14159265358979323846;

	inline float random_float32() {
		thread_local uint32_t state = 3452529; // seed

		// some weird xorshift algorithm for fast randomness
		// https://en.wikipedia.org/wiki/Xorshift
		state ^= state << 13;
		state ^= state >> 17;
		state ^= state << 5;

		return static_cast<float>(state) / static_cast<float>(UINT32_MAX);
	}

	inline double random_double64() {
		thread_local uint64_t state = 345252564789; // seed

		// some weird xorshift algorithm for fast randomness
		// https://en.wikipedia.org/wiki/Xorshift
		state ^= state << 13;
		state ^= state >> 7;
		state ^= state << 17;

		return (state >> 11) * (1.0 / 9007199254740992.0); // 1.0 / 2^53 (max double val)
	}

	inline vec3 random_unit_vector() {
		const float u = random_float32(), v = random_float32();

		// generate a random spherical coordinate
		const double theta = 2 * PI * u;
		const double phi = acos(2 * v - 1);

		// convert to cartesian coords
		double sphi = sin(phi), cost = cos(theta), sint = sin(theta);

		// no need to multiply by rho because we don't really care how far out it is.
		const double x = sphi * cost, y = sphi * sint, z = 2 * v - 1;
		
		return vec3(x, y, z);
	}
}