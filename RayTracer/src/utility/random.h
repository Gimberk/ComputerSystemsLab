#pragma once

#include <cstdint>
#include <numbers>
#include <cmath>

namespace utility {
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
		const double theta = 2 * std::numbers::pi * u;
		const double phi = acos(2 * v - 1);

		// convert to cartesian coords
		const double x = sin(phi) * cos(theta), y = sin(phi) * sin(theta), z = sqrt(u);
		
		return vec3(x, y, z);
	}
}