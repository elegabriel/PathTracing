#include "../scene.h"
#include "../core/camera.h"

#ifndef __INTEGRATOR_H__
#define __INTEGRATOR_H__

class Integrator {
public:
	Integrator(Film *film, const size_t &depth = 3) :m_film(film), m_depth(depth) { count = 0; }

	bool render(const Scene* scene,const Camera* camera, Sampler* sampler);
	Spectrum Li(const Scene *scene, const Ray &ray) ;

	Film *m_film;
	Int m_depth;
	Int count;
};



#endif // !__INTEGRATOR_H__