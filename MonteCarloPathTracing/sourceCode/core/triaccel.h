#include "../define.h"
#include "point.h"
#include "vector.h"
#include "ray.h"

#ifndef __TRIACCEL_H__
#define __TRIACCEL_H__

static const uint32_t KNoTriangleFlag = 0xFFFFFFFF;

/**
* \brief Pre-computed triangle representation based on Ingo Wald's TriAccel layout.
*
* Fits into three 16-byte cache lines if single precision floats are used.
* The k parameter is also used for classification during kd-tree construction.
* \ingroup librender
*/
struct TriAccel {
	uint32_t k;
	Float n_u;
	Float n_v;
	Float n_d;

	Float a_u;
	Float a_v;
	Float b_nu;
	Float b_nv;

	Float c_nu;
	Float c_nv;
	uint32_t shapeIndex;
	uint32_t primIndex;

	/// Construct from vertex data. Returns '1' if there was a failure
	inline int load(const Point &A, const Point &B, const Point &C);

	/// Fast ray-triangle intersection test
	inline bool rayIntersect(const Ray &ray, Float mint, Float maxt,
		Float &u, Float &v, Float &t) const;
};

inline int TriAccel::load(const Point &A, const Point &B, const Point &C) {
	static const int waldModulo[4] = { 1, 2, 0, 1 };

	Vector b = C - A, c = B - A, N = Normalize(product(c, b)); 

	k = 0;
	/* Determine the largest projection axis */
	for (int j = 0; j<3; j++) {
		if (std::abs(N[j]) > std::abs(N[k])) 
			k = j;
	}

	uint32_t u = waldModulo[k],
		v = waldModulo[k + 1];
	const Float n_k = N[k],
		denom = b[u] * c[v] - b[v] * c[u];

	if (denom == 0) {
		k = 3;
		return 1;
	}

	/* Pre-compute intersection calculation constants */
	n_u = N[u] / n_k;
	n_v = N[v] / n_k;
	n_d = dot(Vector(A), N) / n_k;
	b_nu = b[u] / denom;
	b_nv = -b[v] / denom;
	a_u = A[u];
	a_v = A[v];
	c_nu = c[v] / denom;
	c_nv = -c[u] / denom;
	return 0;
}

inline bool TriAccel::rayIntersect(const Ray &ray, Float mint, Float maxt,
	Float &u, Float &v, Float &t) const {

#if 0
	static const MM_ALIGN16 int waldModulo[4] = { 1, 2, 0, 1 };
	const int ku = waldModulo[k], kv = waldModulo[k + 1];
	/* Get the u and v components */
	const Float o_u = ray.o[ku], o_v = ray.o[kv], o_k = ray.o[k],
		d_u = ray.d[ku], d_v = ray.d[kv], d_k = ray.d[k];
#else
	Float o_u, o_v, o_k, d_u, d_v, d_k;
	switch (k) {
	case 0:
		o_u = ray.o[1];
		o_v = ray.o[2];
		o_k = ray.o[0];
		d_u = ray.d[1];
		d_v = ray.d[2];
		d_k = ray.d[0];
		break;
	case 1:
		o_u = ray.o[2];
		o_v = ray.o[0];
		o_k = ray.o[1];
		d_u = ray.d[2];
		d_v = ray.d[0];
		d_k = ray.d[1];
		break;
	case 2:
		o_u = ray.o[0];
		o_v = ray.o[1];
		o_k = ray.o[2];
		d_u = ray.d[0];
		d_v = ray.d[1];
		d_k = ray.d[2];
		break;
	default:
		return false;
	}
#endif


#if defined(MTS_DEBUG_FP)
	if (d_u * n_u + d_v * n_v + d_k == 0)
		return false;
#endif

	/* Calculate the plane intersection (Typo in the thesis?) */
	t = (n_d - o_u*n_u - o_v*n_v - o_k) /
		(d_u * n_u + d_v * n_v + d_k);

	if (t < mint || t > maxt)
		return false;

	/* Calculate the projected plane intersection point */
	const Float hu = o_u + t * d_u - a_u;
	const Float hv = o_v + t * d_v - a_v;

	/* In barycentric coordinates */
	u = hv * b_nu + hu * b_nv;
	v = hu * c_nu + hv * c_nv;
	return u >= 0 && v >= 0 && u + v <= 1.0f;
}


#endif // !__TRIACCEL_H__



