#include "../integrators/integrator.h"
#include "../scene.h"

#ifndef __RENDER_H__
#define __RENDER_H__

class Renderer{
public:
	Renderer(Sampler* s, Camera* c, Integrator* i);
	Spectrum Li(const Scene *scene, const Ray &ray, const Sample *sample);
	void Render(const Scene *scene);
private:
	Integrator *m_integrator;
	Camera *m_camera;
	Sampler *m_sampler;
};
#endif // !__RENDER_H_
