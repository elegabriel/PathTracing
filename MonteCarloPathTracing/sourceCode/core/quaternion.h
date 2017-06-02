#include "vector.h"


#ifndef __QUATERNION_H__
#define __QUATERNION_H__

struct Quaternion {
	// Quaternion Public Methods
	Quaternion() { v = Vector(0., 0., 0.); w = 1.f; }
	Quaternion &operator+=(const Quaternion &q) {
		v += q.v;
		w += q.w;
		return *this;
	}
	friend Quaternion operator+(const Quaternion &q1, const Quaternion &q2) {
		Quaternion ret = q1;
		return ret += q2;
	}
	Quaternion &operator-=(const Quaternion &q) {
		v -= q.v;
		w -= q.w;
		return *this;
	}
	friend Quaternion operator-(const Quaternion &q1, const Quaternion &q2) {
		Quaternion ret = q1;
		return ret -= q2;
	}
	Quaternion &operator*=(Float f) {
		v *= f;
		w *= f;
		return *this;
	}
	Quaternion operator*(Float f) const {
		Quaternion ret = *this;
		ret.v *= f;
		ret.w *= f;
		return ret;
	}
	Quaternion &operator/=(Float f) {
		v /= f;
		w /= f;
		return *this;
	}
	Quaternion operator/(Float f) const {
		Quaternion ret = *this;
		ret.v /= f;
		ret.w /= f;
		return ret;
	}
	Transform ToTransform() const;
	Quaternion(const Transform &t);

	// Quaternion Public Data
	Vector v;
	Float w;
};


Quaternion Slerp(Float t, const Quaternion &q1, const Quaternion &q2);

// Quaternion Inline Functions
inline Quaternion operator*(Float f, const Quaternion &q) {
	return q * f;
}


inline Float Dot(const Quaternion &q1, const Quaternion &q2) {
	return dot(q1.v, q2.v) + q1.w * q2.w;
}


inline Quaternion Normalize(const Quaternion &q) {
	return q / sqrtf(Dot(q, q));
}



#endif // !__QUATERNION_H__





