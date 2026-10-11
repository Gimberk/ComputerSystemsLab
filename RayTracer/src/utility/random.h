#pragma once

#include "vec3.h"

#include <atomic>
#include <cmath>
#include <cstdint>
#include <numbers>
#include <random>

namespace utility {
	constexpr double PI = 3.14159265358979323846;

	// used to prevent all threads from having the same starting seed.
	inline uint64_t make_seed() {
		static std::atomic<uint64_t> seed_counter(1);

		uint64_t seed = seed_counter.fetch_add(1);

		// mix seeds
		seed ^= seed >> 30;
		seed *= 0xbf58476d1ce5eb9ULL;
		seed ^= seed >> 27;
		seed *= 0x94d049bb133111ebULL;
		seed ^= seed >> 31;

		return seed;
	}

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
		thread_local uint64_t state = make_seed(); // seed

		// some weird xorshift algorithm for fast randomness
		// https://en.wikipedia.org/wiki/Xorshift
		state ^= state << 13;
		state ^= state >> 7;
		state ^= state << 17;

		uint64_t scrambled = state * 0x2545F4914F6CDD1DULL;

		return (scrambled >> 11) * (1.0 / 9007199254740992.0); // 1.0 / 2^53 (max double val)
	}

	// min and max are INCLUSIVE
	inline int random_integer_bounds(const int min, const int max) {
		return min + static_cast<int>(random_float32() * (max - min + 1));
	}
}