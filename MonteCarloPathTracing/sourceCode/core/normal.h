#include "vector.h"

#ifndef __NORMAL_H_
#define __NORMAL_H_

struct Normal : public Vector {
	/** \brief Construct a new normal without initializing it.
	*
	* This construtor is useful when the normal will either not
	* be used at all (it might be part of a larger data structure)
	* or initialized at a later point in time. Always make sure
	* that one of the two is the case! Otherwise your program will do
	* computations involving uninitialized memory, which will probably
	* lead to a difficult-to-find bug.
	*/
	Normal() { }

	/// Initialize the vector with the specified X and Z components
	Normal(const Float x, const Float y, const Float z) : Vector(x, y, z) { }

	/// Initialize all components of the the normal with the specified value
	explicit Normal(const Float val) : Vector(val) { }

	/// Construct a normal from a vector data structure
	Normal(const Vector &v) : Vector(v.x, v.y, v.z) { }

	/// Assign a vector to this normal
	void operator=(const Vector &v) {
		x = v.x; y = v.y; z = v.z;
	}

	Normal operator+(const Normal &n) const {
		return Normal(x + n.x, y + n.y, z + n.z);
	}
};

inline Normal normalize(const Normal &n) {
	return n / n.length();
}

#endif // __NORMAL_H_

