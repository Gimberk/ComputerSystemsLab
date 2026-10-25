#pragma once

#include <algorithm>
#include <cmath>
#include <iostream>
#include <numbers>
#include <functional>
#include <type_traits>

constexpr double PI = 3.14159265358979323846;

class alignas(double) vec3 {
public:
	double x, y, z;

	vec3() : x(0), y(0), z(0) {}
	vec3(double x, double y, double z) : x(x), y(y), z(z) {}

	const double* data() const { return &x; }
	double* data() { return &x; }

	vec3 operator-() const { return vec3(-x, -y, -z); }

	vec3& operator+=(const vec3& other) {
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	vec3& operator-=(const vec3& other) {
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	vec3& operator/=(const double v) {
		x /= v;
		y /= v;
		z /= v;
		return *this;
	}

	vec3& operator*=(const double v) {
		x *= v;
		y *= v;
		z *= v;
		return *this;
	}

	double& operator[](const int v) {
		return component(*this, v);
	}

	const double& operator[](const int v) const {
		return component(*this, v);
	}

	bool operator==(const vec3& v) const {
		return x == v.x && y == v.y && z == v.z;
	}

	#ifndef _MSC_VER
	bool operator!=(const vec3 v) const{
		return !((*this) == v); 
	}
	#endif
	
	double length() const { return sqrt(length_squared()); }

	double length_squared() const { return x * x + y * y + z * z; }

	bool near_zero() const { return length() < 0.05; }
};

static_assert(std::is_standard_layout_v<vec3>, "vec3 must be a standard layout class!");

using point3 = vec3;

struct vec3_hash {
	std::size_t operator()(const point3& other) const {
		size_t h1 = std::hash<double>{}(other.x);
		size_t h2 = std::hash<double>{}(other.y);
		size_t h3 = std::hash<double>{}(other.z);

		size_t seed = h1;
		seed ^= h2 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		seed ^= h3 + 0x9e3779b9 + (seed << 6) + (seed >> 2);

		return seed;
	}
};

// inline some functions as smaller functions can be copied a ton to save processing time

inline double to_radians(double degrees) {
	return degrees * (PI / 180);
}

inline double to_degrees(double radians) {
	return radians * (180 / PI);
}

inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
	return out << v.x << ' ' << v.y << ' ' << v.z;
}

inline vec3 operator+(const vec3& u, const vec3& v) {
	return vec3(u.x + v.x, u.y + v.y, u.z + v.z);
}

inline vec3 operator-(const vec3& u, const vec3& v) {
	return u + (-v);
}

inline vec3 operator*(const vec3& v, double t) {
	return vec3(v.x * t, v.y * t, v.z * t);
}

inline vec3 operator*(const vec3& u, const vec3& v) {
	return vec3(u.x * v.x, u.y * v.y, u.z * v.z);
}

inline vec3 operator*(double t, const vec3& v) {
	return v * t;
}

inline vec3 operator/(const vec3& v, double t) {
	return v * (1 / t);
}

inline double dot(const vec3& u, const vec3& v) {
	// u dot v = ||u|| * ||v|| * cos(theta)
	// or just: u * v
	vec3 res = (u * v);
	return res.x + res.y + res.z;
}

inline vec3 cross(const vec3& u, const vec3& v) {
	// just some random math stuff i guess. something about matrices and ijk.
	return vec3(u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x);
}

inline vec3 unit_vector(const vec3& v) {
	return v / v.length();
}

inline vec3 quantize(const point3& p, double epsilon = 1e-4) {
	return vec3(std::round(p.x * (1.0 / epsilon)) * epsilon, 
				std::round(p.y * (1.0 / epsilon)) * epsilon, 
				std::round(p.z * (1.0 / epsilon)) * epsilon);
}

inline double& component(vec3& u, const int i) {
	switch (i) {
	case 0: return u.x;
	case 1: return u.y;
	default: return u.z;
	}
}

inline const double component(const vec3& u, const int i) {
	switch (i) {
	case 0: return u.x;
	case 1: return u.y;
	default: return u.z;
	}
}