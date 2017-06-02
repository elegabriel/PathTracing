#include "structure.h"

face group::getFace(const size_t &index) const {
	if (index >= f_vector.size())
		TRACE_BUG("index %zd is out of range.\n", index);
	return f_vector[index];
}

void group::setFaceBB(const size_t &index, const boundingBox &bb)
{
	f_vector[index].setBB(bb);
	this->bb = Union(this->bb, bb);
}

void face::setBB(boundingBox bb)
{
	this->bb = bb;
	p[0] = (bb.max[0] + bb.min[0]) / 2;
	p[1] = (bb.max[1] + bb.min[1]) / 2;
	p[2] = (bb.max[2] + bb.min[2]) / 2;
}
