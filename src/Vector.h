// Author: Peiyao Li
// Date:   Jan 8 2023
#pragma once
#include<cmath>

class Vec3f;
typedef Vec3f Color3f;
typedef Vec3f Point3f;

const float EPS = 1e-6;

inline bool isEqualf(float a, float b)
{
	if (fabs(a - b) < EPS)
		return true;
	return false;
}

// 基础向量计算 Vec2f和Vec3f
class Vec3f
{
public:
	union {
		struct { float x, y, z; };
		struct { float r, g, b; };
	};

	Vec3f(): x(0.0f), y(0.0f), z(0.0f) {};
	Vec3f(float a, float b, float c) { this->x = a; this->y = b; this->z = c; }
	Vec3f(const Vec3f& vec) { *this = vec; }

	inline bool operator == (const Vec3f& vec) { return isEqualf(x, vec.x) && isEqualf(y, vec.y) && isEqualf(z, vec.z); }
	inline bool operator != (const Vec3f& vec) { return !(isEqualf(x, vec.x) && isEqualf(y, vec.y) && isEqualf(z, vec.z)); }

	inline Vec3f& operator = (const Vec3f& vec) { x = vec.x; y = vec.y; z = vec.z; return *this; }

	inline Vec3f& operator += (float num) { x += num; y += y; z += z; return *this;	}
	inline Vec3f& operator += (const Vec3f& vec) { x += vec.x; y += vec.y; z += vec.z; return *this; }

	inline Vec3f& operator -= (float num) { x -= num; y -= num; z -= num; return *this; }
	inline Vec3f& operator -= (const Vec3f& vec) { x -= vec.x; y -= vec.y; z -= vec.z; return *this; }

	inline Vec3f& operator *= (float num) { x *= num; y *= num; z *= num; return *this; }
	inline Vec3f& operator *= (const Vec3f& vec) { x *= vec.x; y *= vec.y; z *= vec.z; return *this; }

	inline Vec3f& operator /= (float num) { x /= num; y /= num; z /= num; return *this; }
	inline Vec3f& operator /= (const Vec3f& vec) { x /= vec.x; y /= vec.y; z /= vec.z; return *this; }

	inline float length() {
		return sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
	}
	inline Vec3f& normalize() {
		*this /= this->length();
		return *this;
	}

	void clamp(float lower, float upper)
	{
		if (x > upper)x = upper;
		if (x < lower)x = lower;
		if (y > upper)y = upper;
		if (y < lower)y = lower;
		if (z > upper)z = upper;
		if (z < lower)z = lower;
	}


	void gammaCorrection(float const& gamma) {
		this->r = std::pow(this->r, gamma);
		this->g = std::pow(this->g, gamma);
		this->b = std::pow(this->b, gamma);
	};

private:

};

inline Vec3f operator + (float num, const Vec3f& vec) { return Vec3f(num + vec.x, num + vec.y, num + vec.z); }
inline Vec3f operator + (const Vec3f& u, const Vec3f& v) { return Vec3f(u.x + v.x, u.y + v.y, u.z + v.z); }

inline Vec3f operator - (const Vec3f& vec, float num) { return Vec3f(vec.x - num, vec.y - num, vec.z - num); }
inline Vec3f operator - (float num, const Vec3f& vec) { return Vec3f(num - vec.x, num - vec.y, num - vec.z); }
inline Vec3f operator - (const Vec3f& vec, const Vec3f& v) { return Vec3f(vec.x - v.x, vec.y - v.y, vec.z - v.z); }

inline Vec3f operator * (const Vec3f& vec, float num) { return Vec3f(vec.x * num, vec.y * num, vec.z * num); }
inline Vec3f operator * (float num, const Vec3f& vec) { return Vec3f(num * vec.x, num * vec.y, num * vec.z); }
inline Vec3f operator * (const Vec3f& u, const Vec3f& v) { return Vec3f(u.x * v.x, u.y * v.y, u.z * v.z); }

inline Vec3f operator / (const Vec3f& vec, float num) { return Vec3f(vec.x / num, vec.y / num, vec.z / num); }
inline Vec3f operator / (float num, const Vec3f& vec) { return Vec3f(num / vec.x, num / vec.y, num / vec.z); }
inline Vec3f operator / (const Vec3f& u, const Vec3f& v) { return Vec3f(u.x / v.x, u.y / v.y, u.z / v.z); }


inline Vec3f cross(const Vec3f& u, const Vec3f& v) {
	return Vec3f(u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x);
}

inline float dot(const Vec3f& u, const Vec3f& v) {
	return u.x * v.x + u.y * v.y + u.z * v.z;
}

inline float length(const Vec3f& vec) {
	return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

inline Vec3f normalize(const Vec3f& vec) {
	return vec / length(vec);
}

inline Vec3f min(const Vec3f& u, const Vec3f& v)
{
	return Vec3f(std::fmin(u.x, v.x), std::fmin(u.y, v.y), std::fmin(u.z, v.z));
}

inline Vec3f max(const Vec3f& u, const Vec3f& v)
{
	return Vec3f(std::fmax(u.x, v.x), std::fmax(u.y, v.y), std::fmax(u.z, v.z));
}








class Vec2f
{
public:
	union {
		struct { float u, v; };
		struct { float x, y; };
	};

	Vec2f() : x(0.0f), y(0.0f){};
	Vec2f(float a, float b) { this->x = a; this->y = b; }
	Vec2f(const Vec2f& vec) { *this = vec; }

	inline bool operator == (const Vec2f& vec) { return isEqualf(x, vec.x) && isEqualf(y, vec.y); }
	inline bool operator != (const Vec2f& vec) { return !(isEqualf(x, vec.x) && isEqualf(y, vec.y)); }

	inline Vec2f& operator = (const Vec2f& vec) { x = vec.x; y = vec.y; return *this; }

	inline Vec2f& operator += (float num) { x += num; y += y; return *this; }
	inline Vec2f& operator += (const Vec2f& vec) { x += vec.x; y += vec.y; return *this; }

	inline Vec2f& operator -= (float num) { x -= num; y -= num; return *this; }
	inline Vec2f& operator -= (const Vec2f& vec) { x -= vec.x; y -= vec.y; return *this; }

	inline Vec2f& operator *= (float num) { x *= num; y *= num; return *this; }
	inline Vec2f& operator *= (const Vec2f& vec) { x *= vec.x; y *= vec.y; return *this; }

	inline Vec2f& operator /= (float num) { x /= num; y /= num; return *this; }
	inline Vec2f& operator /= (const Vec2f& vec) { x /= vec.x; y /= vec.y; return *this; }


	inline Vec2f& fract() {
		this->x -= floor(this->x);
		this->y -= floor(this->y);
		return *this;
	}

private:

};

inline Vec2f operator + (float num, const Vec2f& vec) { return Vec2f(num + vec.x, num + vec.y); }
inline Vec2f operator + (const Vec2f& u, const Vec2f& v) { return Vec2f(u.x + v.x, u.y + v.y); }

inline Vec2f operator - (const Vec2f& vec, float num) { return Vec2f(vec.x - num, vec.y - num); }
inline Vec2f operator - (float num, const Vec2f& vec) { return Vec2f(num - vec.x, num - vec.y); }
inline Vec2f operator - (const Vec2f& vec, const Vec2f& v) { return Vec2f(vec.x - v.x, vec.y - v.y); }

inline Vec2f operator * (const Vec2f& vec, float num) { return Vec2f(vec.x * num, vec.y * num); }
inline Vec2f operator * (float num, const Vec2f& vec) { return Vec2f(num * vec.x, num * vec.y); }
inline Vec2f operator * (const Vec2f& u, const Vec2f& v) { return Vec2f(u.x * v.x, u.y * v.y); }

inline Vec2f operator / (const Vec2f& vec, float num) { return Vec2f(vec.x / num, vec.y / num); }
inline Vec2f operator / (float num, const Vec2f& vec) { return Vec2f(num / vec.x, num / vec.y); }
inline Vec2f operator / (const Vec2f& u, const Vec2f& v) { return Vec2f(u.x / v.x, u.y / v.y); }


inline Vec2f fract(const Vec2f& vec) {
	return Vec2f(vec.x - floor(vec.x), vec.y - floor(vec.y));
}

inline Vec2f min(const Vec2f& u, const Vec2f& v)
{
	return Vec2f(std::fmin(u.x, v.x), std::fmin(u.y, v.y));
}

inline Vec2f max(const Vec2f& u, const Vec2f& v)
{
	return Vec2f(std::fmax(u.x, v.x), std::fmax(u.y, v.y));
}