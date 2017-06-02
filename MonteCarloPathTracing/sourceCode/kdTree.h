#include "geometry.h"

#ifndef __KDTREE_H__
#define __KDTREE_H__

struct kdNode {
	void init(Float p, uint32_t a) {
		splitPos = p;
		splitAxis = a;
		rightChild = (1 << 29) - 1;
		hasLeftChild = 0;
	}
	void initLeaf() {
		splitAxis = 3;
		rightChild = (1 << 29) - 1;
		hasLeftChild = 0;
	}
	// kdNode Data
	Float splitPos;
	uint32_t splitAxis : 2;
	uint32_t hasLeftChild : 1, rightChild : 29;
	boundingBox bb;
};

template <typename NodeData> class kdTree {
public:
	// kdTree Public Methods
	kdTree(const std::vector<NodeData> &data);
	~kdTree() {
		for (size_t i = 0; i<nNodes; ++i)
		{
			nodeData[i].~NodeData();
		}
		delete[] nodes;
		delete[] nodeData;
		//FreeAligned(nodes);
		//FreeAligned(nodeData);
	}
	template <typename LookupProc> void Lookup(const Point3 &p,
		LookupProc &process, Float &maxDistSquared) const;

	template <typename LookupProc> void Lookup_my(const Point3 &p,
		LookupProc &process, Float &maxDistSquared) const;
	// [tag]
	size_t size() const {
		return nextFreeNode;
	}

	// [tag]
	NodeData &at_data(size_t i) {
		assert(i >= 0 && i < nextFreeNode);
		return nodeData[i];
	}
	kdNode &at_node(size_t i) {
		assert(i >= 0 && i < nextFreeNode);
		return nodes[i];
	}

private:
	// kdTree Private Methods
	void recursiveBuild(uint32_t nodeNum, size_t start, size_t end,
		const NodeData **buildNodes);
	template <typename LookupProc> void privateLookup(uint32_t nodeNum,
		const Point3 &p, LookupProc &process, Float &maxDistSquared) const;

	template <typename LookupProc> void privateLookup_my(uint32_t nodeNum,
		const Point3 &p, LookupProc &process, Float &maxDistSquared) const;

	// kdTree Private Data
	kdNode *nodes;
	NodeData *nodeData;
	uint32_t nNodes, nextFreeNode;
};


template <typename NodeData> struct CompareNode {
	CompareNode(Int a) { axis = a; }
	Int axis;
	bool operator()(const NodeData *d1, const NodeData *d2) const {
		return d1->p[axis] == d2->p[axis] ? (d1 < d2) :
			d1->p[axis] < d2->p[axis];
	}
};



// kdTree Method Definitions
template <typename NodeData>
kdTree<NodeData>::kdTree(const std::vector<NodeData> &d) {
	nNodes = d.size();
	nextFreeNode = 1;
	nodes = new kdNode[nNodes];
	nodeData = new NodeData[nNodes];
	//nodes = AllocAligned<kdNode>(nNodes);
	//nodeData = AllocAligned<NodeData>(nNodes);
	std::vector<const NodeData *> buildNodes(nNodes, NULL);
	for (uint32_t i = 0; i < nNodes; ++i)
		buildNodes[i] = &d[i];
	// Begin the kdTree building process
	recursiveBuild(0, 0, nNodes, &buildNodes[0]);
}


template <typename NodeData> void
kdTree<NodeData>::recursiveBuild(uint32_t nodeNum, size_t start, size_t end,
	const NodeData **buildNodes) {
	// Create leaf node of kd-tree if we've reached the bottom
	if (start + 1 == end) {
		nodes[nodeNum].initLeaf();
		new(nodeData + nodeNum)NodeData(*buildNodes[start]);
		return;
	}

	// Choose split direction and partition data

	// Compute bounds of data from _start_ to _end_
	boundingBox bound;

	for (size_t i = start; i < end; ++i)
		bound = Union(bound, buildNodes[i]->bb);
	size_t splitAxis = bound.MaximumExtent();
	size_t splitPos = (start + end) / 2;
	std::nth_element(&buildNodes[start], &buildNodes[splitPos],
		&buildNodes[end], CompareNode<NodeData>(splitAxis));

	// Allocate kd-tree node and continue recursively
	nodes[nodeNum].init(buildNodes[splitPos]->p[splitAxis], splitAxis);
	//added record bb
	nodes[nodeNum].bb = bound;
	new(nodeData + nodeNum)NodeData(*buildNodes[splitPos]);
	if (start < splitPos) {
		nodes[nodeNum].hasLeftChild = 1;
		uint32_t childNum = nextFreeNode++;
		recursiveBuild(childNum, start, splitPos, buildNodes);
	}
	if (splitPos + 1 < end) {
		nodes[nodeNum].rightChild = nextFreeNode++;
		recursiveBuild(nodes[nodeNum].rightChild, splitPos + 1,
			end, buildNodes);
	}
}


template <typename NodeData> template <typename LookupProc>
void kdTree<NodeData>::Lookup(const Point3 &p, LookupProc &proc,
	Float &maxDistSquared) const {
	privateLookup(0, p, proc, maxDistSquared);
}

template <typename NodeData> template <typename LookupProc>
void kdTree<NodeData>::Lookup_my(const Point3 &p, LookupProc &proc,
	Float &maxDistSquared) const {
	privateLookup_my(0, p, proc, maxDistSquared);
}

template <typename NodeData> template <typename LookupProc>
void kdTree<NodeData>::privateLookup(uint32_t nodeNum, const Point3 &p,
	LookupProc &process, Float &maxDistSquared) const {
	kdNode *node = &nodes[nodeNum];
	// Process kd-tree node's children
	Int axis = node->splitAxis;
	if (axis != 3) {
		Float dist2 = (p[axis] - node->splitPos) * (p[axis] - node->splitPos);
		if (p[axis] <= node->splitPos) {
			if (node->hasLeftChild)
				privateLookup(nodeNum + 1, p, process, maxDistSquared);
			if (dist2 < maxDistSquared && node->rightChild < nNodes)
				privateLookup(node->rightChild, p, process, maxDistSquared);
		}
		else {
			if (node->rightChild < nNodes)
				privateLookup(node->rightChild, p, process, maxDistSquared);
			if (dist2 < maxDistSquared && node->hasLeftChild)
				privateLookup(nodeNum + 1, p, process, maxDistSquared);
		}
	}

	// Hand kd-tree node to processing function
	Float dist2 = DistanceSquared(nodeData[nodeNum].p, p);
	if (dist2 < maxDistSquared)
		process(p, nodeData[nodeNum], dist2, maxDistSquared);
}

template <typename NodeData> template <typename LookupProc>
void kdTree<NodeData>::privateLookup_my(uint32_t nodeNum, const Point3 &p,
	LookupProc &process, Float &maxDistSquared) const {
	kdNode *node = &nodes[nodeNum];
	// Hand kd-tree node to processing function
	Float dist2 = DistanceSquared(nodeData[nodeNum].p, p);
	if (dist2 < maxDistSquared)
		process(p, nodeData[nodeNum], dist2, maxDistSquared);
	// Process kd-tree node's children
	Int axis = node->splitAxis;
	if (axis != 3) {
		Float dist2 = (p[axis] - node->splitPos) * (p[axis] - node->splitPos);
		if (p[axis] <= node->splitPos) {
			if (node->hasLeftChild)
				privateLookup_my(nodeNum + 1, p, process, maxDistSquared);
			if (dist2 < maxDistSquared && node->rightChild < nNodes)
				privateLookup_my(node->rightChild, p, process, maxDistSquared);
		}
		else {
			if (node->rightChild < nNodes)
				privateLookup_my(node->rightChild, p, process, maxDistSquared);
			if (dist2 < maxDistSquared && node->hasLeftChild)
				privateLookup_my(nodeNum + 1, p, process, maxDistSquared);
		}
	}
}

#endif // __KDTREE_H__
