#pragma once

#include <cmath>
#include <iostream>

class vec3 {
public:
	double x, y, z;

	vec3() : x(0), y(0), z(0) {}
	vec3(double x, double y, double z) : x(x), y(y), z(z) {}

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

	bool operator==(const vec3 v) {
		return x == v.x && y == v.y && z == v.z;
	}

	bool operator!=(const vec3 v) {
		return !((*this) == v);
	}
	
	double length() const { return sqrt(length_squared()); }

	double length_squared() const { return x * x + y * y + z * z; }

	bool near_zero() const { return length() < 0.05; }
};

using point3 = vec3;

// inline some functions as smaller functions can be copied a ton to save processing time

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