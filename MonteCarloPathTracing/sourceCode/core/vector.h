#include "../define.h"
#include "point.h"

#ifndef __VECTOR_H_
#define __VECTOR_H_

class Vector2 {
public:
	Vector2(Float _x, Float _y) :x(_x), y(_y) {}
	Float x, y;
};

class Vector3 {
public:
	Vector3() :x(0), y(0), z(0) {}
	Vector3(const Float _x , const Float _y , const Float _z) :x(_x), y(_y), z(_z) {}
	Vector3(const Float _x) :x(_x), y(_x), z(_x) {}
	Vector3(const Vector3& v) :x(v.x), y(v.y), z(v.z) {}

	Vector3(const Point3 &p) :x(p.x), y(p.y), z(p.z) {}

	Vector3 operator+(const Vector3 &v) const{
		return Vector3(x+v.x, y+v.y, z+v.z);
	}

	Vector3 operator-(const Vector3 &v) const {
		return Vector3(x - v.x, y - v.y, z - v.z);
	}
	Vector3 operator-() const {
		return Vector3 (-x, -y, -z);
	}

	Vector3 operator%(Vector3 b) const {
		return Vector3(y*b.z - z*b.y, z*b.x - x*b.z, x*b.y - y*b.x);
	}

	Vector3 const &operator+=(const Vector3 v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	Vector3 const &operator-=(const Vector3 v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}
	Vector3 const &operator*=(Float f){
		x *= f;
		y *= f;
		z *= f;
		return *this;
	}

	Vector3 const operator*(const Float f) const {
		return Vector3(x*f, y*f, z*f);
	}

	Vector3 const &operator/=(Float f) {
		if (f == 0)
			TRACE_BUG("devision is zero!");
		x /= f;
		y /= f;
		z /= f;
		return *this;
	}

	Float& operator[](size_t index) {
		if (index > 2) {
			TRACE_BUG("index %zd is out of range.", index);
		}
		return (&x)[index];
	}

	Float operator[](size_t index) const {
		if (index > 2) {
			TRACE_BUG("index %zd is out of range.", index);
		}
		return (&x)[index];
	}

	Float lengthSquared() const {
		return x*x + y*y + z*z;
	}

	Float LengthSquared() const {
		return LengthSquared();
	}

	Float length() const {
		return (Float)std::sqrt((Float)lengthSquared());
	}

	Float Length() const {
		return length();
	}

	Vector3 operator/(Float f) const {
		if(f == 0)
			TRACE_BUG("Vector3: Division by zero!");
		Float recip = (Float)1 / f;
		return Vector3(x * recip, y * recip, z * recip);
	}

	Float operator*(const Vector3 v)const {
		return x*v.x + y*v.y + z*v.z;
	}

	friend Vector3 operator*(Float f, Vector3 v) {
		return Vector3(v.x*f, v.y*f, v.z*f);
	}
	
	friend Point3 operator+(Point3 p, Vector3 v) {
		return Point3(p.x + v.x, p.y + v.y, p.z + v.z);
	}

	Float x, y, z;
};

inline Vector3 product(const Vector3 &v1, const Vector3 &v2) {
	return Vector3(
		v1.y*v2.z - v1.z*v2.y,
		v1.z*v2.x - v1.x*v2.z,
		v1.x*v2.y - v1.y*v2.x
	);
}

inline Float dot(const Vector3 &v1, const Vector3 &v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

inline Vector3 Normalize(const Vector3 &v) {
	return v / v.length();
}

class Vector4 {
public:
	Vector4(Float _x, Float _y, Float _z, Float _w) :x(_x), y(_y), z(_z), w(_w) {}
	Float x, y, z, w;
};

typedef Vector3 Vector;



#endif // !__VECTOR_H_
