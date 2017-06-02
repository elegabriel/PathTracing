#include "normal.h"

#ifndef __INTERSECTION_H__
#define __INTERSECTION_H__

struct Intersection {
public:
	Intersection():isValid(false),t(FLT_MAX) {}

	vertex texture;
	bool isValid;
	Float t;
	Int material;
	Normal normal;

};

#endif // !__INTERSECTION_H__
