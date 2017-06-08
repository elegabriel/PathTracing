#include "scene.h"
#include "render\film.h"
#include "core\camera.h"
#include "core\sampler.h"
#include "integrators\integrator.h"
#include "render\renderer.h"

int main(int argc, char** argv)
{
	// 场景导入
	Scene scene(Spectrum(0.f));
	if (!scene.readObj("scene02.obj"))
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
	Camera* camera = CreatePerspectiveCamera(cam2world, &film, 45.f, Point(2.6633f, 9.8534f, 24.0849f));

	//Camera* camera = CreatePerspectiveCamera(cam2world, &film, 45.f, Point(1.92f, 8.44f,18.f));

	//向下15度之前
	//Camera* camera = CreatePerspectiveCamera(cam2world, &film, 45.f, Point(1.92f, 3.24f,18.f));
	//Camera* camera = CreatePerspectiveCamera(cam2world, &film, 45.f, Point(1.92f, 3.24f,12.34f));
	//Camera* camera = CreatePerspectiveCamera(cam2world, &film, 90.f, Point(1.92f, 3.0f, 10.f));

	camera->xResolution = film.xResolution;
	camera->yResolution = film.yResolution;

	// Sampler
	Sampler* sampler = CreateHaltonSampler(&film, camera, 4096);

	// Integrator (film, path depth)
	Integrator integrator(&film, 36);

	// Renderere
	Renderer render(sampler, camera, &integrator);

	render.Render(&scene);

	//Save the image
	film.saveImage("film");

	//delete camera;
	//delete sampler;
	return 0;
}
