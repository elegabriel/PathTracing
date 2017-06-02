#include "transform.h"
#include "quaternion.h"



Transform Transform::translate(const Vector & v)
{
	Matrix4x4 m(1, 0, 0, v.x,
		0, 1, 0, v.y,
		0, 0, 1, v.z,
		0, 0, 0, 1);
	Matrix4x4 minv(1, 0, 0, -v.x,
		0, 1, 0, -v.y,
		0, 0, 1, -v.z,
		0, 0, 0, 1);
	return Transform(m, minv);
}

Transform Transform::scale(const Float & x, const Float & y, const Float & z)
{
	Matrix4x4 m(x, 0, 0, 0,
		0, y, 0, 0,
		0, 0, z, 0,
		0, 0, 0, 1);
	Matrix4x4 minv(1.f / x, 0, 0, 0,
		0, 1.f / y, 0, 0,
		0, 0, 1.f / z, 0,
		0, 0, 0, 1);
	return Transform(m, minv);
}

//added function
Transform Scale(const Float & x, const Float & y, const Float & z) {
	return Transform::scale(x, y, z);
}

Transform Translate(const Vector &v) {
	return Transform::translate(v);
}

Transform Inverse(const Transform &t) {
	return t.inverse();
}



Transform Perspective(const Float &fov, const Float &n, const Float &f) {
	// Perform projective divide
	Matrix4x4 persp = Matrix4x4(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, f / (f - n), -f*n / (f - n),
		0, 0, 1, 0);

	// Scale to canonical viewing volume
	Float invTanAng = 1.f / tanf(Radians(fov) / 2.f);
	return Scale(invTanAng, invTanAng, 1) * Transform(persp);
}


// AnimatedTransform Method Definitions
//void AnimatedTransform::Decompose(const Matrix4x4 &m, Vector *T,
//	Quaternion *Rquat, Matrix4x4 *S) {
//	// Extract translation _T_ from transformation matrix
//	T->x = m.m[0][3];
//	T->y = m.m[1][3];
//	T->z = m.m[2][3];
//
//	// Compute new transformation matrix _M_ without translation
//	Matrix4x4 M = m;
//	for (int i = 0; i < 3; ++i)
//		M.m[i][3] = M.m[3][i] = 0.f;
//	M.m[3][3] = 1.f;
//
//	// Extract rotation _R_ from transformation matrix
//	Float norm;
//	int count = 0;
//	Matrix4x4 R = M;
//	do {
//		// Compute next matrix _Rnext_ in series
//		Matrix4x4 Rnext;
//		Matrix4x4 Rit = Inverse(Transpose(R));
//		for (int i = 0; i < 4; ++i)
//			for (int j = 0; j < 4; ++j)
//				Rnext.m[i][j] = 0.5f * (R.m[i][j] + Rit.m[i][j]);
//
//		// Compute norm of difference between _R_ and _Rnext_
//		norm = 0.f;
//		for (int i = 0; i < 3; ++i) {
//			Float n = fabsf(R.m[i][0] - Rnext.m[i][0]) +
//				fabsf(R.m[i][1] - Rnext.m[i][1]) +
//				fabsf(R.m[i][2] - Rnext.m[i][2]);
//			norm = maxFloat(norm, n);
//		}
//		R = Rnext;
//	} while (++count < 100 && norm > .0001f);
//	// XXX TODO FIXME deal with flip...
//	*Rquat = Quaternion(R);
//
//	// Compute scale _S_ using rotation and original matrix
//	
//	*S = Matrix4x4::Mul(Inverse(R), M);
//}

void AnimatedTransform::Interpolate(Float time, Transform *t) const {
	// Handle boundary conditions for matrix interpolation
	if (!actuallyAnimated || time <= startTime) {
		*t = *startTransform;
		return;
	}
	if (time >= endTime) {
		*t = *endTransform;
		return;
	}
	Float dt = (time - startTime) / (endTime - startTime);
	// Interpolate translation at _dt_
	Vector trans = (1.f - dt) * T[0] + dt * T[1];

	// Interpolate rotation at _dt_
	Quaternion rotate = Slerp(dt, R[0], R[1]);

	// Interpolate scale at _dt_
	Matrix4x4 scale;
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			scale.m[i][j] = Lerp(dt, S[0].m[i][j], S[1].m[i][j]);

	// Compute interpolated matrix as product of interpolated components
	*t = Transform::translate(trans) * rotate.ToTransform() * Transform(scale);
}


//boundingBox  AnimatedTransform::MotionBounds(const boundingBox &b,
//	bool useInverse) const {
//
//	if (!actuallyAnimated) {
//		Transform ts = startTransform->getInverseMatrix();
//		return ts(b);
//	}
//	boundingBox ret;
//	const int nSteps = 128;
//	for (int i = 0; i < nSteps; ++i) {
//		Transform t;
//		Float time = Lerp(Float(i) / Float(nSteps - 1), startTime, endTime);
//		Interpolate(time, &t);
//		if (useInverse)
//			t = Inverse(t); 
//		ret = Union(ret, t(b));
//	}
//	return ret;
//}


void AnimatedTransform::operator()(const Ray &r, Ray &tr) const {
	if (!actuallyAnimated || r.time <= startTime)
		(*startTransform)(r, tr);
	else if (r.time >= endTime)
		(*endTransform)(r, tr);
	else {
		Transform t;
		Interpolate(r.time, &t);
		t(r, tr);
	}
	tr.time = r.time;
}

//
//void AnimatedTransform::operator()(const RayDifferential &r,
//	RayDifferential *tr) const {
//	if (!actuallyAnimated || r.time <= startTime)
//		(*startTransform)(r, tr);
//	else if (r.time >= endTime)
//		(*endTransform)(r, tr);
//	else {
//		Transform t;
//		Interpolate(r.time, &t);
//		t(r, tr);
//	}
//	tr->time = r.time;
//}


//Point AnimatedTransform::operator()(Float time, const Point &p) const {
//	if (!actuallyAnimated || time <= startTime)
//		return (*startTransform)(p);
//	else if (time >= endTime)
//		return (*endTransform)(p);
//	Transform t;
//	Interpolate(time, &t);
//	return t(p);
//}


//Vector AnimatedTransform::operator()(Float time, const Vector &v) const {
//	if (!actuallyAnimated || time <= startTime)
//		return (*startTransform)(v);
//	else if (time >= endTime)
//		return (*endTransform)(v);
//	Transform t;
//	Interpolate(time, &t);
//	return t(v);
//}


//Ray AnimatedTransform::operator()(const Ray &r) const {
//	Ray ret;
//	(*this)(r, ret);
//	return ret;
//}

