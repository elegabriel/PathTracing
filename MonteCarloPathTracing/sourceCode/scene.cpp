#include "scene.h"
#include "core\matrix.h"
#include "core\intersection.h"
#include "kdTree.h"

Scene::Scene(const Spectrum &backgroundcolor) :m_background_color(backgroundcolor)
{
	g_vector.push_back(group("default"));
}

void faceKdTree::insert(face * f)
{
	if (f) {
		faceNode *ptrNode = node;
		while (ptrNode) {
			ptrNode = ptrNode->next;
		}
		ptrNode = new faceNode;
		ptrNode->f = f;
		ptrNode->next = NULL;
		num++;
	}
	else
		TRACE_BUG("parameter entity is NULL.");
}

/*unfinished*/
//void faceKdTree::buildKdTree(size_t spDim)
//{
//	Float minFloat = FLT_MAX, maxFloat = FLT_MIN;
//	faceNode *ptrNode = node;
//	splitDim = spDim%VERTEX_DIMENSION; // %3
//
//	if (num > maxNum) {
//		while (ptrNode)
//		{
//			face *f = ptrNode->f;
//			if (f->min[splitDim] > maxFloat)
//				maxFloat = f->min[splitDim];
//			if (f->min[splitDim] < maxFloat)
//				minFloat = f->min[splitDim];
//		}
//		splitPlane = (maxFloat + minFloat) / 2;
//	}
//}


bool Scene::readObj(std::string fileName) {
	std::ifstream file(fileName);
	if (!file) {
		TRACE_BUG("file \"%s\" can't be found.\n", fileName.c_str());
		return false;
	}

	char buf[MAX_LINE_LENGTH] = { 0 };
	char str[MAX_LINE_LENGTH] = { 0 };
	Int line = 0;

	size_t v_num = 0, vn_num = 0, vt_num = 0;

	/*Read file .mtl first*/
	while (file)
	{
		line++;
		if (!file.getline(buf, MAX_LINE_LENGTH)) {
			if (file)
				TRACE_BUG("get line %d failed.\n", line);
			break;
		}

		if (buf[0] == 'm' && sscanf_s(buf, "mtllib %s", str, MAX_LINE_LENGTH)) {
			readMtl(str);
		}
		/*Vertices and vertex normals*/
		else if (buf[0] == 'v') {
			/*vertices*/
			if (buf[1] == ' ') {
				v_num++;
				continue;
			}
			/*normals*/
			else if (buf[1] == 'n') {
				vn_num++;
				continue;
			}
			/*textures*/
			else if (buf[1] == 't')
			{
				vt_num++;
				continue;
			}
		}
	}

	v_vector.reserve(v_num);
	vn_vector.reserve(vn_num);
	vt_vector.reserve(vt_num);

	/*Parse Analysis*/
	line = 0;
	file.clear();
	file.seekg(std::ios::beg);
	bool smooth = false;
	Int materialIndex = -1;
	/*currentGroup = default group*/
	group *currentGroup = &g_vector[0];
	while (file) {
		Float x, y, z;

		line++;
		if (!file.getline(buf, MAX_LINE_LENGTH)) {
			if (file)
				TRACE_BUG("get line %d failed.\n", line);
			break;
		}

		/*Geometric vertices*/
		if (buf[0] == '\0')
			continue;

		/*Vertices and vertex normals*/
		else if (buf[0] == 'v') {
			/*vertices*/
			if (buf[1] == ' ' && sscanf_s(&buf[2], "%f %f %f", &x, &y, &z) == 3) {
				v_vector.push_back(vertex(x, y, z));
				continue;
			}
			/*normals*/
			else if (buf[1] == 'n' && sscanf_s(&buf[3], "%f %f %f", &x, &y, &z) == 3) {
				vn_vector.push_back(Normal(x, y, z));
				continue;
			}
			/*textures*/
			else if (buf[1] == 't')
			{
				Int num = sscanf_s(&buf[3], "%f %f %f", &x, &y, &z);
				if (num == 2)
					vt_vector.push_back(vertex(x, y, 0, true));
				else if (num == 3)
					vt_vector.push_back(vertex(x, y, z));
				else
					TRACE_BUG("vt is not 2d or 3d.\n");
				continue;
			}
		}

		/*Face*/
		else if (buf[0] == 'f') {
			face f;
			char *nxtStr;
			char *tok1 = strtok_s(&buf[1], " ", &nxtStr);
			while (tok1) {
				char *nxtStr2;
				Int index;
				char *tok2;

				/*get vertex index*/
				tok2 = strtok_s(tok1, "/", &nxtStr2);
				sscanf_s(tok2, "%d", &index);
				f.pushV(index);

				/*get texture index*/
				tok2 = strtok_s(NULL, "/", &nxtStr2);
				if (tok2 != NULL && *nxtStr2 != '\0') {
					sscanf_s(tok2, "%d", &index);
					f.pushVt(index);
				}

				/*if the format likes 1//1*/
				else if (tok2 != NULL) {
					sscanf_s(tok2, "%d", &index);
					f.pushVt(-1);
					f.pushVn(index);
					tok1 = strtok_s(NULL, " ", &nxtStr);
					continue;
				}
				else
					f.pushVt(-1);

				/*get normal index*/
				tok2 = strtok_s(NULL, "/", &nxtStr2);
				if (tok2 != NULL) {
					sscanf_s(tok2, "%d", &index);
					f.pushVn(index);
				}
				else
					f.pushVn(-1);

				tok1 = strtok_s(NULL, " ", &nxtStr);
			}
			if (f.getVertexNum() == 4) {
				face f2;

				f2.pushV(f.v_vector[2]);
				f2.pushV(f.v_vector[3]);
				f2.pushV(f.v_vector[0]);

				f2.pushVn(f.vn_vector[2]);
				f2.pushVn(f.vn_vector[3]);
				f2.pushVn(f.vn_vector[0]);

				f2.pushVt(f.vt_vector[2]);
				f2.pushVt(f.vt_vector[3]);
				f2.pushVt(f.vt_vector[0]);

				f2.setSmooth(smooth);
				f2.setMaterial(materialIndex);
				currentGroup->pushF(f2);

				f.v_vector.pop_back();
				f.vn_vector.pop_back();
				f.vt_vector.pop_back();
			}
			
			f.setSmooth(smooth);
			f.setMaterial(materialIndex);
			currentGroup->pushF(f);
			continue;
		}

		/*Group*/
		else if (buf[0] == 'g' && sscanf_s(&buf[2], "%s", str, MAX_LINE_LENGTH)) {
			size_t i = 0;
			for (; i < g_vector.size(); i++)
			{
				if (g_vector[i].getGroupName() == str) {
					currentGroup = &g_vector[i];
					break;
				}
			}
			if (i == g_vector.size())
				g_vector.push_back(group(str, group::groupType::g));
			currentGroup = &g_vector[i];
			continue;
		}
		/*Object*/
		else if (buf[0] == 'o' && sscanf_s(&buf[2], "%s", str, MAX_LINE_LENGTH)) {
			size_t i = 0;
			for (; i < g_vector.size(); i++)
			{
				if (g_vector[i].getGroupName() == str) {
					currentGroup = &g_vector[i];
					break;
				}
			}
			if (i == g_vector.size())
				g_vector.push_back(group(str, group::groupType::o));
			currentGroup = &g_vector[i];
			continue;
		}
		/*Smooting group*/
		else if (buf[0] == 's') {
			if (!memcmp(buf, "s off", 5) || (buf[1] == ' ' && buf[2] == '0'))
				smooth = false;
			else
				smooth = true;
			continue;
		}

		/*Comment*/
		else if (buf[0] == '#')
			continue;
		/*Material name*/
		else if (buf[0] == 'u' && sscanf_s(buf, "usemtl %s", str, MAX_LINE_LENGTH)) {
			size_t i = 0;
			for (size_t i = 0; i < mtl_vector.size(); i++)
			{
				if (mtl_vector[i].name == str) {
					materialIndex = (Int)i;
					break;
				}
			}
			if (i == mtl_vector.size()) {
				TRACE_BUG("file %s, line %d, material \"%s\" is not found.\n", fileName.c_str(), line, str);
				materialIndex = -1;
			}
			continue;
		}
		/*Material library*/
		else if (buf[0] == 'm' && sscanf_s(buf, "mtllib %s", str, MAX_LINE_LENGTH))
			continue;
		else
			TRACE_BUG("line %d isn't defined.\n", line);
	}

	setFaceBB();
	for (size_t i = 0; i < g_vector.size(); i++)
	{
		if (!g_vector[i].buildKdTree())
			TRACE_BUG("Build kdTree failed. Group %zd.\n", i);
	}

	return true;
}

bool Scene::readMtl(std::string fileName) {
	std::ifstream file(fileName);
	if (!file) {
		TRACE_BUG("file \"%s\" can't be found.", fileName.c_str());
		return false;
	}

	char buf[MAX_LINE_LENGTH] = { 0 };
	Int line = 1;

	while (file)
	{
		if (!file.getline(buf, MAX_LINE_LENGTH)) {
			if (file)
				TRACE_BUG("get line %d failed.\n", line);
			break;
		}
		char str[MAX_LINE_LENGTH] = { 0 };
		if (buf[0] == 'n' && sscanf_s(buf, "newmtl %s", str, MAX_LINE_LENGTH)) {
			Material mtl;
			mtl.name = str;
			while (file) {
				std::streampos pos = file.tellg();
				Float a, b, c;
				size_t illum;
				if (!file.getline(buf, MAX_LINE_LENGTH)) {
					if (file)
						TRACE_BUG("get line %d failed.\n", line);
					break;
				}
				if (buf[0] == 'K') {

					if (buf[1] == 'a' && sscanf_s(buf, "Ka %f %f %f", &a, &b, &c) == 3)
					{
						mtl.Ka[0] = a;
						mtl.Ka[1] = b;
						mtl.Ka[2] = c;
					}
					else if (buf[1] == 'd' && sscanf_s(buf, "Kd %f %f %f", &a, &b, &c) == 3)
					{
						mtl.Kd[0] = a;
						mtl.Kd[1] = b;
						mtl.Kd[2] = c;
					}
					else if (buf[1] == 's' && sscanf_s(buf, "Ks %f %f %f", &a, &b, &c) == 3)
					{
						mtl.Ks[0] = a;
						mtl.Ks[1] = b;
						mtl.Ks[2] = c;
					}
				}
				else if (buf[0] == 'N' && buf[1] == 's'&& sscanf_s(buf, "Ns %f", &a))
					mtl.Ns = a;
				else if (buf[0] == 'N' && buf[1] == 'i'&& sscanf_s(buf, "Ni %f", &a))
					mtl.Ni = a;
				else if (buf[0] == 'T' && sscanf_s(buf, "Tr %f", &a))
					mtl.Tr = a;
				else if (buf[0] == 'i' && sscanf_s(buf, "illum %zd", &illum))
					mtl.illum = illum;
				else if (buf[0] == 'n') {
					file.seekg(pos);
					break;
				}
				else
					TRACE_BUG("line %d isn't defined.\n", line);
				line++;
			}
			mtl_vector.push_back(mtl);
		}
		line++;
	}
	file.close();

	return true;
}

bool Scene::writeObj(std::string filename) {
	std::ofstream file(filename);
	bool pre_smooth = false;
	Int materialIndex = -1;
	for (size_t i = 0; i<v_vector.size(); i++)
		file << v_vector[i];
	for (size_t i = 0; i < vn_vector.size(); i++)
		file << "vn " << vn_vector[i][0] << ' ' << vn_vector[i][1] << ' ' << vn_vector[i][2] << std::endl;
	for (size_t i = 0; i < vt_vector.size(); i++)
	{
		if (vt_vector[i].isTwoValue())
			file << "vt " << vt_vector[i][0] << ' ' << vt_vector[i][1] << std::endl;
		else
			file << "vt " << vt_vector[i][0] << ' ' << vt_vector[i][1] << ' ' << vt_vector[i][2] << std::endl;
	}

	file << "s off" << std::endl;
	for (std::vector<group>::iterator iter_group = g_vector.begin(); iter_group != g_vector.end(); iter_group++)
	{
		if (iter_group->getFaceNum() == 0)
			continue;

		if (iter_group->getGroupType() == group::groupType::o)
			file << "o ";
		else if (iter_group->getGroupType() == group::groupType::g)
			file << "g ";
		file << iter_group->getGroupName() << std::endl;

		for (size_t i = 0; i < iter_group->getFaceNum(); i++)
		{
			if (iter_group->getFace(i).isSmooth() != pre_smooth)
			{
				if (pre_smooth)
					file << "s off" << std::endl;
				else
					file << "s 1" << std::endl;
				pre_smooth = !pre_smooth;
			}
			Int currentMaterial = iter_group->getFace(i).getMaterial();
			if (currentMaterial != materialIndex && currentMaterial != -1) {
				materialIndex = currentMaterial;
				file << "usemtl " << mtl_vector[currentMaterial].name << std::endl;
			}
			file << iter_group->getFace(i);
		}
	}
	file.close();


	return true;
}

void Scene::setFaceBB()
{
	/*bounding box: maxX, maxY, maxZ, minX, minY, minZ*/
	
	for (size_t i = 0; i < g_vector.size(); i++)
	{
		for (size_t j = 0; j < g_vector[i].getFaceNum(); j++)
		{
			boundingBox bb;
			face f = g_vector[i].getFace(j);

			Point3 pt[3];
			for (size_t k = 0; k < f.getVertexNum(); k++)
			{
				size_t index = f.getVertexIndex(k);
				vertex v = v_vector[index];
				Point3 p(v[0], v[1], v[2]);
				if (k < 3)
					pt[k] = Point3(v[0], v[1], v[2]);

				bb = Union(bb, p);
			}
			f.ta.load(pt[0], pt[1], pt[2]);
			g_vector[i].setFaceBB(j, bb);
		}

	}
}


static inline bool IntersectBB(const boundingBox &bounds, const Ray &ray) {

	Float nearT = -std::numeric_limits<Float>::infinity();
	Float farT = std::numeric_limits<Float>::infinity();

	for (int i = 0; i<3; i++) {
		const Float origin = ray.o[i];
		const Float minVal = bounds.min[i], maxVal = bounds.max[i];

		if (abs(ray.d[i]) < FLOAT_EPSILON) {
			/* The ray is parallel to the planes */
			if (origin < minVal || origin > maxVal)
				return false;
		}
		else {
			/* Calculate intersection distances */
			Float t1 = (minVal - origin) * ray.dRcp[i];
			Float t2 = (maxVal - origin) * ray.dRcp[i];

			if (t1 > t2)
				std::swap(t1, t2);

			nearT = std::max(t1, nearT);
			farT = std::min(t2, farT);

			if (!(nearT <= farT))
				return false;
		}
	}

	return true;


	//Float tmin, tmax;




	//bool d_x_zero = std::abs(ray.d.x) < FLOAT_EPSILON;
	//bool d_y_zero = std::abs(ray.d.y) < FLOAT_EPSILON;
	//bool d_z_zero = std::abs(ray.d.z) < FLOAT_EPSILON;

	//if ((d_x_zero && (ray.o.x < bounds.min.x || ray.o.x > bounds.max.x)) ||
	//	(d_y_zero && (ray.o.y < bounds.min.y || ray.o.y > bounds.max.y)) ||
	//	(d_z_zero && (ray.o.z < bounds.min.z || ray.o.z > bounds.max.z)))
	//	return false;

	//if (!d_x_zero && !d_y_zero) {
	//	Vector invDir(1.f / ray.d.x, 1.f / ray.d.y, 0.0f);
	//	uint32_t dirIsNeg[3] = { invDir.x < 0, invDir.y < 0, invDir.z < 0 };
	//	// Check for ray intersection against $x$ and $y$ slabs
	//	tmin = (bounds[dirIsNeg[0]].x - ray.o.x) * invDir.x;
	//	tmax = (bounds[1 - dirIsNeg[0]].x - ray.o.x) * invDir.x;
	//	Float tymin = (bounds[dirIsNeg[1]].y - ray.o.y) * invDir.y;
	//	Float tymax = (bounds[1 - dirIsNeg[1]].y - ray.o.y) * invDir.y;
	//	if ((tmin > tymax) || (tymin > tmax))
	//		return false;
	//	if (tymin > tmin) tmin = tymin;
	//	if (tymax < tmax) tmax = tymax;
	//}
	//else if(d_x_zero && !d_y_zero)
	//{
	//	if (ray.d.y < 0) {
	//		tmin = (bounds.max.y - ray.o.y)/ray.d.y;
	//		tmax = (bounds.min.y - ray.o.y) / ray.d.y;
	//	}
	//	else {
	//		tmin = (bounds.min.y - ray.o.y) / ray.d.y;
	//		tmax = (bounds.max.y - ray.o.y) / ray.d.y;
	//	}
	//}
	//else if (d_y_zero && !d_x_zero) {
	//	if (ray.d.x < 0) {
	//		tmin = (bounds.max.x - ray.o.x) / ray.d.x;
	//		tmax = (bounds.min.x - ray.o.x) / ray.d.x;
	//	}
	//	else {
	//		tmin = (bounds.min.x - ray.o.x) / ray.d.x;
	//		tmax = (bounds.max.x - ray.o.x) / ray.d.x;
	//	}
	//}
	//else {
	//	if (ray.d.z < 0) {
	//		tmin = (bounds.max.z - ray.o.z) / ray.d.z;
	//		tmax = (bounds.min.z - ray.o.z) / ray.d.z;
	//	}
	//	else {
	//		tmin = (bounds.min.z - ray.o.z) / ray.d.z;
	//		tmax = (bounds.max.z - ray.o.z) / ray.d.z;
	//	}
	//	return (tmin < ray.maxt) && (tmax > ray.mint);
	//}

	//if (!d_z_zero) {
	//	// Check for ray intersection against $z$ slab
	//	Float invZ = 1.f / ray.d.z;
	//	bool isNegZ = invZ < 0;
	//	Float tzmin = (bounds[isNegZ].z - ray.o.z) * invZ;
	//	Float tzmax = (bounds[1 - isNegZ].z - ray.o.z) * invZ;
	//	if ((tmin > tzmax) || (tzmin > tmax))
	//		return false;
	//	if (tzmin > tmin)
	//		tmin = tzmin;
	//	if (tzmax < tmax)
	//		tmax = tzmax;
	//	//return (tmin < ray.maxt) && (tmax > ray.mint);
	//}

	//return (tmin < ray.maxt) && (tmax > ray.mint);
}

//intersect with triangle
static inline bool IntersectF(const face &f, const Ray &ray,const Scene *scene, Intersection * isect) {
	
	vertex a = scene->v_vector[f.v_vector[0]-1];
	vertex b = scene->v_vector[f.v_vector[1]-1];
	vertex c = scene->v_vector[f.v_vector[2]-1];

	Normal an, bn, cn;
	//vertex at, bt, ct;

	//if(f.vn_vector[0]>0)
		an = scene->vn_vector[f.vn_vector[0]-1];
	//if(f.vn_vector[1]>0)
		bn = scene->vn_vector[f.vn_vector[1]-1];
	//if(f.vn_vector[2]>0)
		cn = scene->vn_vector[f.vn_vector[2]-1];

	/*if (f.vt_vector[0] > 0)
		at = scene->vt_vector[f.vt_vector[0] - 1];
	if (f.vt_vector[1] > 0)
		bt = scene->vt_vector[f.vt_vector[1] - 1];
	if (f.vt_vector[2] > 0)
		ct = scene->vt_vector[f.vt_vector[2] - 1];*/
	//Normal an = scene->(norIndex);

	/*Vector3 e1(b[0] - a[0], b[1] - a[1], b[2] - a[2]);
	Vector3 e2(c[0] - b[0], c[1] - b[1], c[2] - b[2]);

	Vector3 normal = product(e1, e2);
	normal = normal / (normal.x + normal.y + normal.z);*/

	Matrix3x3 matA(
		a[0] - b[0], a[0] - c[0], ray.d.x,
		a[1] - b[1], a[1] - c[1], ray.d.y,
		a[2] - b[2], a[2] - c[2], ray.d.z);
	Matrix3x3 matBeta(
		a[0] - ray.o.x, a[0] - c[0], ray.d.x,
		a[1] - ray.o.y, a[1] - c[1], ray.d.y,
		a[2] - ray.o.z, a[2] - c[2], ray.d.z
	);
	Matrix3x3 matGamma(
		a[0] - b[0], a[0] - ray.o.x, ray.d.x,
		a[1] - b[1], a[1] - ray.o.y, ray.d.y,
		a[2] - b[2], a[2] - ray.o.z, ray.d.z
	);
	Matrix3x3 matT(
		a[0] - b[0], a[0] - c[0], a[0] - ray.o.x,
		a[1] - b[1], a[1] - c[1], a[1] - ray.o.y,
		a[2] - b[2], a[2] - c[2], a[2] - ray.o.z
	);

	Float A = matA.det();
	if (std::abs(A) < FLOAT_EPSILON)
	{
		//TRACE_BUG("A %lf is nan or zero.", A);
		return false;
	}
	Float beta = matBeta.det() / A;
	if (beta <= 0)
		return false;
	Float gamma = matGamma.det() / A;
	if (gamma <= 0 || beta+gamma>=1)
		return false;
	Float t = matT.det() / A;

	if (t<ray.mint || t>ray.maxt)
		return false;
	else if (t < isect->t)
	{
		if (!f.isSmooth())
			isect->normal = an;
		else
			isect->normal = an*(1 - beta - gamma) + bn*beta + cn*gamma;

		isect->t = t;
		//isect->texture = at*(1 - beta - gamma) + bt*beta + ct*gamma;
		isect->isValid = true;
		isect->material = f.getMaterial();
		return true;
	}
	else
		return true;
}

bool hitAfterBB(kdTree<face>* kdtree, const size_t index, const Ray&ray, 
	Intersection * its, const Scene *scene) {
	kdNode* node = &kdtree->at_node(index);
	face f = kdtree->at_data(index);

	//leaf node
	if (node->hasLeftChild == 0) {
		return IntersectF(f, ray, scene, its);
	}
	else {
		bool hitLeft = false, hitRight = false, hitThis = false;
		if (node->hasLeftChild && IntersectBB((node + 1)->bb, ray))
			hitLeft = hitAfterBB(kdtree, index + 1, ray, its, scene);
		if (node->rightChild != ((1 << 29) - 1)
			&& IntersectBB((node - index + node->rightChild)->bb, ray))
			hitRight = hitAfterBB(kdtree, node->rightChild, ray, its, scene);
		hitThis = IntersectF(f, ray, scene, its);
		return hitLeft || hitRight || hitThis;
	}
}

bool hit(kdTree<face>* kdtree, const size_t index, const Ray&ray, 
	Intersection * its, const Scene *scene) {
	kdNode node = kdtree->at_node(index);

	if (IntersectBB(node.bb, ray)) {
		//leaf node
		if (node.hasLeftChild == 0) {
			face f = kdtree->at_data(index);
			return IntersectF(f, ray, scene, its);
		}
		else {
			bool hitLeft = false, hitRight = false, hitThis = false;
			if (node.hasLeftChild)
				hitLeft = hit(kdtree, index + 1, ray, its, scene);
			if (node.rightChild != ((1 << 29) - 1))
				hitRight = hit(kdtree, node.rightChild, ray, its, scene);
			face f = kdtree->at_data(index);
			hitThis = IntersectF(f, ray, scene, its);
			return hitLeft || hitRight || hitThis;
		}
	}
	return false;
}

void Scene::Intersect(const Ray & ray, Intersection * isect) const
{
	for (size_t i = 0; i < g_vector.size(); i++)
	{
		/*if (g_vector[i].kdt && IntersectBB(g_vector[i].bb, ray))
			hitAfterBB(g_vector[i].kdt, 0, ray, isect, this);*/
		/*no pre bb interserction version*/
		if (g_vector[i].kdt)
			hit(g_vector[i].kdt, 0, ray, isect, this);
		

		//if (g_vector[i].getFaceNum() == 0 || !IntersectBB(g_vector[i].bb, ray))
		//	continue;
		////kdTree<face>* kdt =  g_vector[i].kdt;
		//for (size_t j = 0; j < g_vector[i].getFaceNum(); j++)
		//{
		//	face f = g_vector[i].getFace(j);

		//	

		//	if (!IntersectBB(f.bb, ray))
		//		continue;
		//	IntersectF(f, ray, this, isect);

		//	/*Float u, v, t;
		//	Float nearT = FLOAT_EPSILON, farT = FLT_MAX;
		//	if (f.ta.rayIntersect(ray, nearT, farT, u, v, t) && t < isect->t) {
		//	isect->t = t;
		//	isect->isValid = true;
		//	isect->material = f.getMaterial();

		//	Normal an = vn_vector[f.vn_vector[0] - 1],
		//	bn = vn_vector[f.vn_vector[1] - 1],
		//	cn = vn_vector[f.vn_vector[2] - 1];
		//	if (!f.isSmooth())
		//	isect->normal = an;
		//	else
		//	isect->normal = an*(1 - u - v) + bn*u + cn*v;
		//	}*/
		//}

	}
}

size_t Scene::getGroupNum() {
	return g_vector.size();
}

group& Scene::getGroup(size_t index) {
	if (index < g_vector.size())
		return g_vector[index];
	else
		TRACE_BUG("index %zd is out of range.\n", index);
	return g_vector[0];
}

size_t Scene::getVertexNum() {
	return v_vector.size();
}

size_t Scene::getNormalNum() {
	return vn_vector.size();
}

vertex Scene::getVertex(size_t index) {
	if (index < v_vector.size())
		return v_vector[index];
	else
		TRACE_BUG("index %zd is out of range.", index);
	return vertex();
}

Normal Scene::getNormal( size_t index) {
	if (index < vn_vector.size())
		return vn_vector[index];
	else
		TRACE_BUG("index %zd is out of range.", index);
	return Normal();
}

Material Scene::getMaterial(size_t index)const
{
	if (index >= mtl_vector.size())
		TRACE_BUG("Material index %zd out of range!", index);
	return mtl_vector[index];
}
