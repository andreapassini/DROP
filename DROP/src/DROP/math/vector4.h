#pragma once

#include <iostream>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include <cmath>
#include <assert.h>

#include "scalar.h"


using namespace std;

namespace VgMath
{

class Vector4
{
public:
	//Scalar x,y,z; // the three coordinates (as fields)
#ifndef ANONYMOUS_STRUCT
	union { Scalar x, r; };
	union { Scalar y, g; };
	union { Scalar z, b; };
	union { Scalar w, a; };
#else
	union
	{
		struct { Scalar x, y, z; w; };
		struct { Scalar r, g, b; a; };
	}
#endif // 

	// constructor
	Vector4(
		Scalar _x
		, Scalar _y
		, Scalar _z
		, Scalar _w
	) :x(_x), y(_y), z(_z), w(_w) {}

	Vector4(Scalar _n) :x(_n), y(_n), z(_n), w(_n) {}

	// empty constructor
	Vector4() :x(0.0), y(0.0), z(0.0), w(0.0) {}

	// accessing the coordinates as an array  READING THEM
	Scalar operator[] (int i) const {
		if (i == 0) return x;
		if (i == 1) return y;
		if (i == 2) return z;
		assert(i == 3);
		return w;
	}

	// accessing the coords WRITING
	Scalar& operator[] (int i) {
		if (i == 0) return x;
		if (i == 1) return y;
		if (i == 2) return z;
		assert(i == 3);
		return w;
	}

	void print() const {
		std::cout
			<< "(" << x
			<< ", " << y
			<< ", " << z
			<< ", " << w
			<< ") ";
	}

	// vector sum
	Vector4 operator + (const Vector4& b) const {
		return Vector4(x + b.x, y + b.y, z + b.z, w + b.w);
	}

	// the UNITARY plus
	Vector4 operator + () const { return *this; }

	// the BINARY minus
	Vector4 operator - (const Vector4& b) const	{
		return Vector4(x - b.x, y - b.y, z - b.z, w - b.w);
	}

	// the UNARY minus
	Vector4 operator - () const {
		return Vector4(-x, -y, -z, -w);
	}

	Vector4 operator * (Scalar k) const {
		return Vector4(k * x, k * y, k * z, k * w);
	}

	// component-wise product
	Vector4 operator * (const Vector4& k) const {
		return Vector4(k.x * x, k.y * y, k.z * z, k.w * w);
	}

	Vector4 operator / (Scalar k) const {
		return Vector4(x / k, y / k, z / k, w / k);
	}

	// versioni "in place":
	void operator += (const Vector4& b) { x += b.x; y += b.y; z += b.z; }
	void operator -= (const Vector4& b) { x -= b.x; y -= b.y; z -= b.z; }
	void operator *= (Scalar k) { x *= k; y *= k; z *= k; }
	void operator /= (Scalar k) { x /= k; y /= k; z /= k; }
};

inline Vector4 operator * (Scalar k, const Vector4& a) {
	return a * k;
}

inline Vector4 mix(Vector4 a, Vector4 b, Scalar t) {
	return a * (1 - t) + b * t;
}

inline Vector4 lerp(Vector4 a, Vector4 b, Scalar t) {
	return mix(a, b, t);
}

}