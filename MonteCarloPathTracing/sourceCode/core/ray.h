#include "../geometry.h" 
#include "vector.h"

#ifndef __RAY_H_
#define __RAY_H_

struct Ray {
	inline Ray() : mint(Epsilon),
		maxt(std::numeric_limits<Float>::infinity()), time(0) {}

	/// Copy constructor (1)
	inline Ray(const Ray &ray)
		: o(ray.o), mint(ray.mint), d(ray.d), maxt(ray.maxt),
		dRcp(ray.dRcp), time(ray.time) {
	}
		
	/// Copy constructor (2)
	inline Ray(const Ray &ray, Float mint, Float maxt)
		: o(ray.o), mint(mint), d(ray.d), maxt(maxt),
		dRcp(ray.dRcp), time(ray.time) { }

	/// Construct a new ray, while not specifying a direction yet
	inline Ray(const Point &o, Float time) : o(o), mint(Epsilon),
		maxt(std::numeric_limits<Float>::infinity()), time(time) {}

	/// Construct a new ray
	inline Ray(const Point &o, const Vector &d, Float time)
		: o(o), mint(Epsilon), d(d),
		maxt(std::numeric_limits<Float>::infinity()), time(time) {
		for (int i = 0; i<3; ++i)
			dRcp[i] = (Float)1 / d[i];
	}

	/// Construct a new ray
	inline Ray(const Point &o, const Vector &d, Float mint, Float maxt,
		Float time) : o(o), mint(mint), d(d), maxt(maxt), time(time) {
		for (int i = 0; i<3; ++i)
			dRcp[i] = (Float)1 / d[i];
	};

	//added
	inline Ray(const Point &o, const Vector &d, Float mint, Float maxt) 
		:o(o), mint(mint), d(d), maxt(maxt), time(time){
		for (int i = 0; i<3; ++i)
			dRcp[i] = (Float)1 / d[i];
	}
	//added
	inline Ray(const Point &o, const Vector &d, Float mint, Float maxt,
		Float time, Int depth) : o(o), mint(mint), d(d), maxt(maxt), time(time), depth(depth) {
		for (int i = 0; i<3; ++i)
			dRcp[i] = (Float)1 / d[i];
	};


	Point operator()(Float &t) const { 
		return o + (d * t); 
	}

	Point o;		// Ray origin
	Vector d;		// Ray direction
	Vector dRcp;	//Componentwise reciprocals of the ray direction
	Float mint;		//Minimum range for intersection tests
	Float maxt;		//Maximum range for intersection tests
	Float time;		//Time value associated with this ray
	Int depth;
};

class RayDifferential : public Ray {
public:
	// RayDifferential Public Methods
	RayDifferential() { hasDifferentials = false; }
	RayDifferential(const Point &org, const Vector &dir, Float start,
		Float end = INFINITY, Float t = 0.f, int d = 0)
		: Ray(org, dir, start, end, t, d) {
		hasDifferentials = false;
	}
	RayDifferential(const Point &org, const Vector &dir, const Ray &parent,
		Float start, Float end = INFINITY)
		: Ray(org, dir, start, end, parent.time, parent.depth + 1) {
		hasDifferentials = false;
	}
	explicit RayDifferential(const Ray &ray) : Ray(ray) {
		hasDifferentials = false;
	}
	/*bool HasNaNs() const {
		return Ray::HasNaNs() ||
			(hasDifferentials && (rxOrigin.HasNaNs() || ryOrigin.HasNaNs() ||
				rxDirection.HasNaNs() || ryDirection.HasNaNs()));
	}*/
	void ScaleDifferentials(Float s) {
		rxOrigin = o + (rxOrigin - o) * s;
		ryOrigin = o + (ryOrigin - o) * s;
		rxDirection = d + (rxDirection - d) * s;
		ryDirection = d + (ryDirection - d) * s;
	}

	// RayDifferential Public Data
	bool hasDifferentials;
	Point rxOrigin, ryOrigin;
	Vector rxDirection, ryDirection;
};




#endif // !__RAY_H_
