#include "integrator.h"

inline void dRcpRay(Ray &ray) {
	for (int i = 0; i < 3; ++i) {
		/*if (std::abs(ray.d[i]) < FLOAT_EPSILON)
			TRACE_BUG("ray->d[%d]: %f\n", i, ray.d[i]);*/
		ray.dRcp[i] = (Float)1 / ray.d[i];
	}
}

inline void GetSamples(Sample *samples, const Int &x, const Int &y, const Int &count) {
	Float u = RadicalInverse(count, 3);
	Float v = RadicalInverse(count, 2);
	samples->imageX = x + u;
	samples->imageY = y + v;
}

bool Integrator::render(const Scene * scene, const Camera * camera, Sampler * sampler)
{
	srand((uint32_t)time(NULL));

	uint32_t totalResolution = m_film->xResolution*m_film->yResolution;

	uint32_t spp = sampler->samplesPerPixel;
#pragma omp parallel for schedule(dynamic, 1) //private(spec) 
	for (int count = 0; count < (int)totalResolution; count++)
	{
		if (count % (totalResolution / 10) == 0) std::cout << "count/num = " << (Float)count / (Float)totalResolution << std::endl;

		//spec here is more clear
		Spectrum spec(0);

		for (size_t i = 0; i < spp; i++)
		{
			Sample sample;
			//sampler->GetMoreSamples(&sample);
			//std::cout << "GetSamples" << std::endl;
			GetSamples(&sample, count%m_film->xResolution, count / m_film->xResolution, i+1);
			Ray ray;
			//std::cout << "GenerateRay" << std::endl;
			camera->GenerateRay((CameraSample)sample, &ray);
			ray.depth = 0;
			//std::cout << "Li" << std::endl;
			//Intersection isect;
			spec += Li(scene, ray);
			//std::cout << "setPixel" << std::endl;
			/*m_film->setPixel(
				Point3(Float(count%m_film->xResolution), Float(count / m_film->xResolution), 0.f),
				spec );*/
		}
		m_film->setPixel(
			Point3(Float(count%m_film->xResolution), Float(count / m_film->xResolution), 0.f),
			spec / (Float)spp);
	}
	return false;
}

#define specular_n 3;



Spectrum Integrator::Li(const Scene * scene, const Ray & ray)
{
	Intersection isect;
	scene->Intersect(ray, &isect);


	if (isect.isValid) {
		Spectrum spec(0.f);
		//hit point
		Point x = ray.o + ray.d*isect.t;
		bool is_hit_face_back = ray.d * isect.normal > FLOAT_EPSILON;

		Material material = scene->getMaterial(isect.material);
		Float diffuse = material.Kd[0] + material.Kd[1] + material.Kd[2];
		Float specular = material.Ks[0] + material.Ks[1] + material.Ks[2];
		Float ambient = material.Ka[0] + material.Ka[1] + material.Ka[2];
		Float Tr = material.Tr;
		bool is_emitter = ambient > FLOAT_EPSILON,
			is_Tr = Tr > FLOAT_EPSILON,
			is_specular = specular > FLOAT_EPSILON,
			is_diffuse = !is_Tr && !is_emitter && (diffuse > FLOAT_EPSILON);
			//is_diffuse = !is_Tr && !is_emitter && (diffuse > FLOAT_EPSILON);
		//return material.Ka + material.Kd + material.Ks;
		//Float p = (Float)rand() / (Float)RAND_MAX;
		
		/*if (p < material.MaxColor())
			p = 1 / material.MaxColor();
		else
			return material.Ka;*/

		Float p = 1.f;
		if (is_emitter)
			return spec += material.Ka;
		//if (ray.depth > m_depth) {
		//	if (is_diffuse) {

		//		//type1 随机找光源 1次
		//		if (diffuse > FLOAT_EPSILON && material.Tr < FLOAT_EPSILON) {
		//			Float dirX = ((Float)rand() / (Float)RAND_MAX) *2.1f - 1.05f;
		//			Float dirZ = ((Float)rand() / (Float)RAND_MAX) *2.1f - 1.05f;
		//			Vector dir(dirX, 9.882112f, dirZ);
		//			dir = Normalize(dir - x);
		//			Ray next_ray(x, dir, ray.time);
		//			dRcpRay(next_ray);
		//			Intersection isect_next;
		//			scene->Intersect(next_ray, &isect_next);
		//			if (isect_next.isValid) {
		//				return scene->getMaterial(isect_next.material).Ka*material.Kd;
		//			}
		//		}
		//		//type1 end

		//		////type2 均匀找光源 100次
		//		//for (size_t i = 0; i < 10; i++)
		//		//{
		//		//	for (size_t j = 0; j < 10; j++)
		//		//	{
		//		//		Float dirX = 0.2f*Float(i + 1) - 1.1f;
		//		//		Float dirZ = 0.2f*Float(j + 1) - 1.1f;
		//		//		Vector dir(dirX, 9.882112f, dirZ);
		//		//		dir = Normalize(dir - x);
		//		//		Ray next_ray(x, dir, ray.time);
		//		//		dRcpRay(next_ray);
		//		//		Intersection isect_next;
		//		//		scene->Intersect(next_ray, &isect_next);
		//		//		if (isect_next.isValid) {
		//		//			spec += scene->getMaterial(isect_next.material).Ka*material.Kd;
		//		//		}
		//		//	}
		//		//}
		//		//return spec / 100.f;
		//		////type2 end
		//	}

		//	return material.Ka;
		//}

		//环境光 不考虑自身diffuse
		

		//透明物体 不考虑本身diffuse
		if (is_Tr) {
			//d - isect.normal * 2 * (isect.normal*(ray.d)) 全反射方向

			//入射
			bool into = !is_hit_face_back;
			Vector nl = !is_hit_face_back ? isect.normal : -isect.normal;
			//折射率
			Float nnt = is_hit_face_back ? material.Ni : 1.f / material.Ni,
				ddn = ray.d*nl, cos2t;
			//全反射
			Ray reflRay(x, ray.d - isect.normal * 2 * (isect.normal*(ray.d)), ray.time);
			dRcpRay(reflRay);
			reflRay.depth = ray.depth + 1;
			if ((cos2t = 1 - nnt*nnt*(1 - ddn*ddn)) < 0) {
				spec += Li(scene, reflRay);
				return spec;
			}

			Vector tdir =
				Normalize(ray.d*nnt - isect.normal*((into ? 1 : -1)*(ddn*nnt + sqrt(cos2t))));
			Ray next_ray(x, tdir, ray.time);
			dRcpRay(next_ray);
			next_ray.depth = ray.depth + 1;
			Float Tr = material.Tr, Re = 1 - Tr, P = 0.25f + 0.5f*Re, RP = Re / P, TP = Tr / (1 - P);
			Float u3 = (Float)rand() / (Float)RAND_MAX;
			spec += ray.depth > 2 ? (u3 < P ?
				Li(scene, reflRay)*RP : Li(scene, next_ray)*TP) :
				Li(scene, reflRay)*Re + Li(scene, next_ray)*Tr;
		}

		//镜面物体
		if (is_specular) {
			if (is_hit_face_back) isect.normal = -isect.normal;

			/*Float u1 = (Float)rand() / (Float)RAND_MAX;
			Float u2 = (Float)rand() / (Float)RAND_MAX;
			Float r1 = 2 * (Float)M_PI * u1, r2 = u2, r2ss = powf(u2, 1.f / (Float)(3 + 1));
			Vector w = ray.d - isect.normal * 2 * (isect.normal*(ray.d)),
			u = Normalize((fabs(w.x)>.1 ? Vector(0.f, 1.f, 0.f) : Vector(1.f, 0.f, 0.f)) % w),
			v = w%u;

			Vector d = Normalize(u*cos(r1)*r2ss + v*sin(r1)*r2ss + w*sqrt(1 - r2));
			Ray next_ray(x, d, ray.time);*/

			Vector dir = ray.d - isect.normal * 2 * (isect.normal*ray.d);
			Ray next_ray(x, dir, ray.time);
			dRcpRay(next_ray);
			next_ray.depth = ray.depth + 1;
			spec += Li(scene, next_ray)*material.Ks;
		}

		// diffuse
		if (is_diffuse) {
			if (is_hit_face_back) isect.normal = -isect.normal;
			Float u1 = (Float)rand() / (Float)RAND_MAX;
			Float u2 = (Float)rand() / (Float)RAND_MAX;
			Float r1 = 2 * (Float)M_PI * u1, r2 = u2, r2sd = sqrtf(u2);
			Vector w = isect.normal,
				u = Normalize((fabs(w.x) > .1 ? Vector(0.f, 1.f, 0.f) : Vector(1.f, 0.f, 0.f)) % w),
				v = w%u;
			Vector d = Normalize(u*cos(r1)*r2sd + v*sin(r1)*r2sd + w*sqrt(1 - r2));
			Ray next_ray(x, d, ray.time);
			dRcpRay(next_ray);
			next_ray.depth = ray.depth + 1;
			//has bug
			spec += Li(scene, next_ray)*material.Kd;
		}

		/*if (ray.depth >= m_depth && diffuse >FLOAT_EPSILON && ) {

		}*/
		//if (ray.depth >= m_depth || (ray.depth > 5 && material.MaxColor() < p))
		//{
		//	/*if (diffuse > FLOAT_EPSILON && material.Tr < FLOAT_EPSILON) {
		//		Float dirX = ((Float)rand() / (Float)RAND_MAX) *2.1f - 1.05f;
		//		Float dirZ = ((Float)rand() / (Float)RAND_MAX) *2.1f - 1.05f;
		//		Vector dir(dirX, 9.882f, dirZ);
		//		dir = Normalize(dir - x);
		//		Ray next_ray(x, dir, ray.time);
		//		dRcpRay(next_ray);
		//		Intersection isect_next;
		//		scene->Intersect(next_ray, &isect_next);
		//		if (isect_next.isValid) {
		//			return scene->getMaterial(isect_next.material).Ka*material.Kd;
		//		}
		//	}*/
		//	return material.Ka;

		//	/*if (isect_next.isValid) {
		//		if(diffuse)
		//			return scene->getMaterial(isect_next.material).Ka*material.Kd;
		//		if (specular)
		//			return scene->getMaterial(isect_next.material).Ka*material.Ks;
		//	}*/
		//	/*if (diffuse)
		//		return material.Kd*0.78f;
		//	if (specular)
		//		return material.Ks*0.78f;
		//	return material.Ka;*/
		//}
		return spec* p;
	}
	else {
		/*if(ray.depth ==0)
		std::cout<<"missed : " << ray.depth << std::endl;*/
		return scene->getBackgroundColor();
	}
}