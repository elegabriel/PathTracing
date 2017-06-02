#include "renderer.h"
#include "../core/sampler.h"

Renderer::Renderer(Sampler * s, Camera * c, Integrator * i) :m_camera(c), m_sampler(s), m_integrator(i)
{
}

void Renderer::Render(const Scene * scene)
{
	m_integrator->render(scene, m_camera, m_sampler);
}
