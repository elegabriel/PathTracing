#include "../define.h"

#ifndef __POINT_H_
#define __POINT_H_

class Point3 {
public:
	Point3() :x(0), y(0), z(0) {};
	Point3(Float f) :x(f), y(f), z(f) {};
	Point3(Float _x, Float _y, Float _z) :x(_x), y(_y), z(_z) {};
	Point3 operator-(Point3 p) {
		return Point3(x - p.x, y - p.y, z - p.z);
	}
	Float &operator[](size_t index) {
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

	Point3 operator/(const Float w) const {
		if (std::isfinite(w) && w == 0) {
			TRACE_BUG("Float w is %f", w);
			return Point3();
		}
		return Point3(x / w, y / w, z / w);
	}

	Point3 operator/=(const Float w) const {
		if (std::isfinite(w) && w == 0) {
			TRACE_BUG("Float w is %f", w);
			return Point3();
		}
		return Point3(x / w, y / w, z / w);
	}

	Point3 operator*(const Float f) const {
		return Point3(x*f, y*f, z*f);
	}

	Point3 operator-(const Point3 p)const {
		return Point3(x - p.x, y - p.y, z - p.z);
	}

	Point3 operator+(const Point3 p)const {
		return Point3(x + p.x, y + p.y, z + p.z);
	}

	Float x, y, z;
	Float Scalar;
};

typedef Point3 Point;

#endif // !__POINT_H_
