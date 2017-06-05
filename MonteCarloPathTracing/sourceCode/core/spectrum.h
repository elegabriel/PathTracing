#include "../define.h"

#ifndef __SPECTRUM_H__
#define __SPECTRUM_H__

class Spectrum {
public:
	Spectrum() {}
	Spectrum(Float a, Float b, Float c):x(a), y(b), z(c) {}
	Spectrum(Float a):x(a), y(a), z(a) {}

	Float &operator[](size_t index) {
		if (index > 2)
			TRACE_BUG("index %zd is out of range.", index);
		return (&x)[index];
	}
	Float operator[](size_t index)const {
		if (index > 2)
			TRACE_BUG("index %zd is out of range.", index);
		return (&x)[index];
	}

	Spectrum operator/(const Float &f)const {
		if (isnan(f) || f < FLOAT_EPSILON)
			TRACE_BUG("Float f %f is nan or zero.", f);
		return Spectrum(x / f, y / f, z / f);
	}

	Spectrum &operator+=(const Spectrum &s){
		x += s.x;
		y += s.y;
		z += s.z;
		return *this;
	}

	Spectrum operator+(const Spectrum &s) {
		return Spectrum(s.x + x, s.y + y, s.z + z);
	}

	Spectrum operator*(const Spectrum &s)const {
		return Spectrum(s.x*x, s.y*y, s.z*z);
	}

	Spectrum operator*(const Float &f) const{
		return Spectrum(x*f, y*f, z*f);
	}

	friend Spectrum operator-(const Float &f, const Spectrum &s) {
		return Spectrum(f - s.x, f - s.y, f - s.z);
	}

protected:
	/*RGB*/
	Float x, y, z;
};

#endif // !__SPECTRUM_H__
