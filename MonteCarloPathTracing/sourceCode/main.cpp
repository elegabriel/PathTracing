#include "scene.h"
#include "render\film.h"
#include "core\camera.h"
#include "core\sampler.h"
#include "integrators\integrator.h"
#include "render\renderer.h"

int main(int argc, char** argv)
{
	// ³¡¾°µ¼Èë
	Scene scene(Spectrum(0.f));
	if (!scene.readObj("scene01.obj"))
		TRACE_BUG("Read obj file failed.\n");
	//sc.writeObj("out.obj");

	Matrix4x4 tz(
		-1.f, 0.f, 0.f, 0.f,
		0.f, -1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f
	);
	Matrix4x4 m(
		1.f, 0.f, 0.f, 0.f,
		0.f, -1.f, 0.f, 5.2f,
		0.f, 0.f, -1.f, 16.7f,
		0.f, 0.f, 0.f, 1.0f
	);
	Transform t(m*tz);
	AnimatedTransform cam2world(&t, 0, &t, 1);
	Film film(500, 500,false);

	Camera* camera = CreatePerspectiveCamera(cam2world, &film, 45.f, Point(0.f,5.0f,14.5f));
	camera->xResolution = film.xResolution;
	camera->yResolution = film.yResolution;

	// Sampler
	Sampler* sampler = CreateHaltonSampler(&film, camera, 4096);

	// Integrator (film, path depth)
	Integrator integrator(&film, 16);

	// Renderere
	Renderer render(sampler, camera, &integrator);

	render.Render(&scene);

	//Save the image
	film.saveImage("film");

	//delete camera;
	//delete sampler;
	return 0;
}
