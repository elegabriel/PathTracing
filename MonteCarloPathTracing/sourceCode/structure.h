#include "define.h"
#include "kdTree.h"
#include "core\ray.h"
#include "core\spectrum.h"
#include "core\triaccel.h"

#ifndef __STRUCTURE_H__
#define __STRUCTURE_H__

class Material {
public:
	Material() {
		name = "";
		Ka = Spectrum(0.0f);
		Kd = Spectrum(0.0f);
		Ks = Spectrum(0.0f);
		Ns = 0.0f;
		Ni = 0.0f;
		Tr = 0.0f;
		illum = 0;
	}

	Float MaxColor()const {
		//Float maxKa = maxFloat(maxFloat(Ka[0], Ka[1]), Ka[2]);
		Float maxKd = maxFloat(maxFloat(Kd[0], Kd[1]), Kd[2]);
		Float maxKs = maxFloat(maxFloat(Ks[0], Ks[1]), Ks[2]);
		return maxFloat(maxFloat( maxKd, maxKs),Tr);
	}
	std::string name;
	/*ambient color*/
	Spectrum Ka;
	/*diffuse color*/
	Spectrum Kd;
	/*speular color*/
	Spectrum Ks;
	/*0-1000*/
	Float Ns;
	/*transparent*/
	Float Tr;
	/**/
	Float Ni;
	/*illumination mode*/
	size_t illum;
private:
};

/*Vertex*/
class vertex{
public:
    
    vertex(){}
    vertex(Float _x, Float _y, Float _z, bool _twoValue = false)
		:x(_x),y(_y),z(_z),twoValue(_twoValue){}
    friend std::ostream &operator<<(std::ostream &os, vertex &v){
        os << "v " << v[0] << ' ' << v[1] << ' ' << v[2] << std::endl;
        return os;
    }
    
    Float &operator[](const size_t index){
		if (index > 2)
			TRACE_BUG("index %zd is out of range.", index);
		return (&x)[index];
    }

	Float operator[](const size_t index) const{
		if (index > 2)
			TRACE_BUG("index %zd is out of range.", index);
		return (&x)[index];
	}
    
    vertex operator-(vertex& v) const{
        return vertex(x - v[0], y - v[1], z - v[2]);
    }
    
	vertex operator+(vertex& v) const{
		return vertex(x + v[0], y + v[1], z + v[2]);
	}

    Float operator*(vertex& v) {
        return Float(x * v[0] + y * v[1] + z * v[2]);
    }
    
	vertex operator*(const Float f) const{
		return vertex(x * f, y * f, z * f);
	}

    vertex operator-() {
        return vertex(-x, -y, -z);
    }
    
	Float length() {
		return sqrt(x * x + y * y + z * z);
	}

	void normalize() {
		Float len = length();
		x = x / len;
		y = y / len;
		z = z / len;
	}
    
    /*friend vertex &operator-(vertex& v1, vertex&v2){
     return vertex(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
     }*/
    
    vertex crossProduct(vertex v) {
        return vertex(
                     y*v[2]-z*v[1],
                     z*v[0]-x*v[2],
                     x*v[1]-y*v[0]
                     );
    }
    
    Float norm() {
		return x*x + y*y + z*z;
    }
    
	bool isTwoValue() {
		return twoValue;
	}

private:
	Float x, y, z;
	bool twoValue;
};

class brdf {

};

/*Face*/
class face{
public:
    friend std::ostream &operator<<(std::ostream &os, face &f){
        os << 'f';
        for (size_t i = 0; i < f.v_vector.size(); i++)
        {
            os << ' ' << f.v_vector[i];
            if (f.vt_vector[i] == -1 && f.vn_vector[i] == -1)
                continue;
            
            if (f.vt_vector[i] != -1)
                os << '/' << f.vt_vector[i];
            else
                os << '/';
            
            if (f.vn_vector[i] != -1)
                os << '/' << f.vn_vector[i];
        }
        os << std::endl;
        return os;
    }
	//index 已经减1，即从0开始
    Int getVertexIndex(const size_t index) const {
        if (index < v_vector.size())
            return v_vector[index] - 1;
        else
			TRACE_BUG("index %zd is out of range.", index);
        return -1;
    }
    
	Int getVertexNormalIndex(const size_t index)const {
		if (index < vn_vector.size())
			return vn_vector[index] - 1;
		else
			TRACE_BUG("index %zd is out of range.", index);
		return -1;
	}

	Int getVertexTexture(const size_t index)const {
		if (index < vt_vector.size())
			return vt_vector[index] - 1;
		else
			TRACE_BUG("index %zd is out of range.", index);
		return -1;
	}

    size_t getVertexNum() {
        return v_vector.size();
    }
    
    /*without checking positive index or not*/
    bool pushV(Int index) {
        v_vector.push_back(index);
        return true;
    }
    
    bool pushVt(Int index) {
        vt_vector.push_back(index);
        return true;
    }
    
    bool pushVn(Int index) {
        vn_vector.push_back(index);
        return true;
    }

	bool isSmooth() const{
		return smooth;
	}
	void setSmooth(bool smooth) {
		this->smooth = smooth;
	}
	void setMaterial(Int material) {
		this->material = material;
	}
	Int getMaterial() const{
		return material;
	}

    Int &operator[](size_t index) {
        if (index < v_vector.size())
            return v_vector[index];
        else if (index < v_vector.size() + vt_vector.size())
            return vt_vector[index - v_vector.size()];
        else if (index < v_vector.size() + vt_vector.size() + vn_vector.size())
            return vn_vector[index - v_vector.size() - vt_vector.size()];
        else
			TRACE_BUG("index %zd is out of range.", index);
        return error_value_int;
    }
    
	void setBB(boundingBox bb);

	/*bounding box*/
	boundingBox bb;
	/*bounding box center*/
	Point3 p;

	std::vector<Int> v_vector;
	std::vector<Int> vt_vector;
	std::vector<Int> vn_vector;

	TriAccel ta;
private:

    /*Geometric vertices, Texture vertices, Vertex normals*/
	//保存原始index，从1开始

    Int error_value_int;
	bool smooth;
	Int material;
};



/*Group*/
class group{
public:
	enum groupType {
		/*Group*/
		g,
		/*Object Name*/
		o
	};

    group(const std::string name, groupType type = groupType::g){
        this->name = name;
		this->type = type;
		kdt = NULL;
    }
    
    bool pushF(face f) {
        f_vector.push_back(f);
        return true;
    }
    
	face getFace(const size_t &index) const;

	void setFaceBB(const size_t &index, const boundingBox &bb);

	std::string getGroupName() {
		return name;
	}
    
    size_t getFaceNum() const {
        return f_vector.size();
    }

	groupType getGroupType() {
		return type;
	}

	void setGroupType(groupType type) {
		this->type = type;
	}

	void intersect(Ray Ray){
	}

	bool buildKdTree() {
		if (f_vector.size() == 0) return true;

		kdt = new kdTree<face>(f_vector);
		if (!kdt)
			return false;
		return true;
	}


	boundingBox bb;
	kdTree<face>* kdt;
private:
    std::string name;
	std::vector<face> f_vector;
	groupType type;
};


#endif /* __STRUCTURE_H__ */
