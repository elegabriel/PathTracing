#include "define.h"
#include "core\point.h"

#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

class boundingBox {
public:
	boundingBox():max(-FLT_MAX),min(FLT_MAX){}
	boundingBox(Point3 p):max(p), min(p) {}

	friend boundingBox Union(const boundingBox& bb,const Point3& p) {
		boundingBox result;
		result.max.x = maxFloat(bb.max.x, p.x);
		result.max.y = maxFloat(bb.max.y, p.y);
		result.max.z = maxFloat(bb.max.z, p.z);
		result.min.x = minFloat(bb.min.x, p.x);
		result.min.y = minFloat(bb.min.y, p.y);
		result.min.z = minFloat(bb.min.z, p.z);
		return result;
	}

	friend boundingBox Union(const boundingBox &b, const boundingBox &b2) {
		boundingBox ret;
		ret.min.x = minFloat(b.min.x, b2.min.x);
		ret.min.y = minFloat(b.min.y, b2.min.y);
		ret.min.z = minFloat(b.min.z, b2.min.z);
		ret.max.x = maxFloat(b.max.x, b2.max.x);
		ret.max.y = maxFloat(b.max.y, b2.max.y);
		ret.max.z = maxFloat(b.max.z, b2.max.z);
		return ret;
	}

	inline const Point3 operator[](size_t i) const {
		assert(i == 0 || i == 1);
		return (&min)[i];
	}

	size_t MaximumExtent() {
		Point3 p = max - min;
		if (p.x > p.y && p.x > p.z)
			return 0;
		else if (p.y > p.z)
			return 1;
		else
			return 2;
	}
	Point3 min, max;
};

#endif // __GEOMETRY_H__
