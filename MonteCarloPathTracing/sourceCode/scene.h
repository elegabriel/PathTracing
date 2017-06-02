//
//  scene.h
//  
//
//  Created by ele on 3/14/17. Modified by ele on 4/26/2017
//  Copyright © 2017 ele. All rights reserved.
//

#include "structure.h"
#include "core\intersection.h"

#ifndef __SCENE_H__
#define __SCENE_H__

struct faceNode {
	face* f;
	faceNode* next;
};

class faceKdTree {
public:
	faceKdTree(size_t maxNum) :
		maxNum(maxNum), node(NULL),num(0){};
	void insert(face* f);
	void buildKdTree(size_t spDim);

	faceNode* node;
	/*face point in the node*/
	size_t num;
	/*max face point in the node*/
	size_t maxNum;
	/*split split*/
	Float splitPlane;
	/*split dimensian*/
	size_t splitDim;
	int splitDimInt;

	faceKdTree* left;
	faceKdTree* mid;
	faceKdTree* right;
	faceKdTree* parent;
};


class Scene{
public:
	Scene(const Spectrum &backgroundcolor = Spectrum(0.f));
	bool readObj(std::string fileName);
	bool readMtl(std::string fileName);
	bool writeObj(std::string filename);
    
	size_t getGroupNum();
	size_t getVertexNum();
	size_t getNormalNum();

	group& getGroup(size_t index);
	vertex getVertex(size_t index);
	Normal getNormal(size_t index);
	Material getMaterial(size_t index)const;

	void setFaceBB();

	void Intersect(const Ray &ray, Intersection *isect) const;

	Spectrum getBackgroundColor() const { return m_background_color; }

	std::vector<group> g_vector;
	std::vector<vertex> v_vector;
	std::vector<Normal> vn_vector;
	std::vector<vertex> vt_vector;
	std::vector<Material> mtl_vector;
private:
	Spectrum m_background_color;
};

#endif /* __SCENE_H__ */
