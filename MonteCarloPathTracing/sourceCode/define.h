//
//  define.h
//  
//
//  Created by ele on 3/14/17. Modified by ele on 4/26/2017
//  Copyright © 2017 ele. All rights reserved.
//

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <sstream>
#include <assert.h>
#include <ctime>
/*use math defines in math.h, like M_PI*/
#define _USE_MATH_DEFINES true
#include <math.h>

#ifndef __DEFINE_H__
#define __DEFINE_H__

/*The max number of a polygon's edges*/
#define MAX_EDGE_NUM 10
#define MAX_LINE_LENGTH 256
#define FLOAT_EPSILON FLT_EPSILON
#define VERTEX_DIMENSION 3
#define FACEKDTREE_MAX_NUM 3
#define RCPOVERFLOW 2.93873587705571876e-39f

//mistuba
#define Epsilon 1e-4f

//#define MAXVERTICESNUM 4

typedef float Float;
typedef int Int;
//typedef unsigned char BYTE;

#define maxFloat(a, b) std::fmaxf((a), (b))
#define minFloat(a, b) std::fminf((a), (b))

#define TRACE_BUG(fmt,...) printf("FUNCTION %s, LINE(%d): "##fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

//#define TRACE_BUG_1 (printf("%s(%d)-<%s>: ",__FILE__, __LINE__, __FUNCTION__), printf)
//#define TRACE_BUG_2(fmt,...) printf("%s(%d)-<%s>: "##fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

// Global Forward Declarations

class Sampler;
class Camera;
class ProjectiveCamera;
class PerspectiveCamera;
class Film;
class boundingBox;
class Integrator;
class Point3;
class RayDifferential;
class Renderer;
class RNG;
class Sampler;
class HaltonSampler;
class Scene;
class Spectrum;
class Material;
class vertex;
class face;
class group;
class AnimatedTransform;
class Vector2;
class Vector3;
class Vector4;

template <typename NodeData> class kdTree;
template <typename NodeData> struct CompareNode;
template <int M, int N, typename T> struct Matrix;

struct Matrix2x2;
struct Matrix3x3;
struct Matrix4x4;
struct Normal;
struct Quaternion;
struct Ray;
struct CameraSample;
struct Sample;
struct Transform;
struct Intersection;
struct kdNode;


inline Float Clamp(Float val, Float low, Float high) {
	if (val < low) return low;
	else if (val > high) return high;
	else return val;
}

//差值
inline Float Lerp(Float t, Float v1, Float v2) {
	return (1.f - t) * v1 + t * v2;
}

inline Float Radians(Float deg) {
	return ((Float)M_PI / 180.f) * deg;
}



//void LatinHypercube(float *samples, uint32_t nSamples, uint32_t nDim,
//	RNG &rng) {
//	// Generate LHS samples along diagonal
//	float delta = 1.f / nSamples;
//	for (uint32_t i = 0; i < nSamples; ++i)
//		for (uint32_t j = 0; j < nDim; ++j)
//			samples[nDim * i + j] = min((i + (rng.RandomFloat())) * delta,
//				OneMinusEpsilon);
//
//	// Permute LHS samples in each dimension
//	for (uint32_t i = 0; i < nDim; ++i) {
//		for (uint32_t j = 0; j < nSamples; ++j) {
//			uint32_t other = j + (rng.RandomUInt() % (nSamples - j));
//			swap(samples[nDim * j + i], samples[nDim * other + i]);
//		}
//	}
//}

//montecarlo.h

//change double to float
inline Float RadicalInverse(const int& _n,const int& _base) {
	int n = _n, base = _base;
	Float val = 0;
	Float invBase = (Float)(1.f / base), invBi = invBase;
	while (n > 0) {
		// Compute next digit of radical inverse
		int d_i = (n % base);
		val += d_i * invBi;
		//n *= invBase;
		n = (Int)(n*invBase);
		invBi *= invBase;
	}
	return val;
}

#endif // !__DEFINE_H__



