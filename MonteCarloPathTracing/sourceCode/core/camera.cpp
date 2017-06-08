#include "camera.h"

Camera::~Camera() {
	delete film;
}

Camera::Camera(const AnimatedTransform &cam2world,
	Float sopen, Float sclose, Film *f)
	: CameraToWorld(cam2world), shutterOpen(sopen), shutterClose(sclose) {
	film = f;
	/*if (CameraToWorld.HasScale())
		TRACE_BUG("Scaling detected in world-to-camera transformation!\n"
			"The system has numerous assumptions, implicit and explicit,\n"
			"that this transform will have no scale factors in it.\n"
			"Proceed at your own risk; your image may have errors or\n"
			"the system may crash as a result of this.");*/
}

Float Camera::GenerateRayDifferential(const CameraSample &sample,
	RayDifferential *rd) const {
	Float wt = GenerateRay(sample, rd);
	// Find ray after shifting one pixel in the $x$ direction
	CameraSample sshift = sample;
	++(sshift.imageX);
	Ray rx;
	Float wtx = GenerateRay(sshift, &rx);
	rd->rxOrigin = rx.o;
	rd->rxDirection = rx.d;

	// Find ray after shifting one pixel in the $y$ direction
	--(sshift.imageX);
	++(sshift.imageY);
	Ray ry;
	Float wty = GenerateRay(sshift, &ry);
	rd->ryOrigin = ry.o;
	rd->ryDirection = ry.d;
	if (wtx == 0.f || wty == 0.f) return 0.f;
	rd->hasDifferentials = true;
	return wt;
}


ProjectiveCamera::ProjectiveCamera(const AnimatedTransform &cam2world,
	const Transform &proj, const Float screenWindow[4], Float sopen,
	Float sclose, Float lensr, Float focald, Film *f)
	: Camera(cam2world, sopen, sclose, f) {
	// Initialize depth of field parameters
	lensRadius = lensr;
	focalDistance = focald;

	// Compute projective camera transformations
	CameraToScreen = proj;
	
	// Compute projective camera screen transformations
	ScreenToRaster =
		Scale(Float(film->xResolution), 
			  Float(film->yResolution), 1.f) *
		Scale(1.f / (screenWindow[1] - screenWindow[0]), 
			  1.f / (screenWindow[2] - screenWindow[3]), 1.f) *
		Translate(Vector(-screenWindow[0], -screenWindow[3], 0.f));
	RasterToScreen = Inverse(ScreenToRaster);
	RasterToCamera = Inverse(CameraToScreen) * RasterToScreen;
	
}

PerspectiveCamera::PerspectiveCamera(const AnimatedTransform & cam2world, 
	const Float screenWindow[4], Float sopen, Float sclose, 
	Float lensr, Float focald, Float fov, Film * film,const Point &orginal)
	: ProjectiveCamera(cam2world, Perspective(fov, 1e-2f, 1000.f),
		screenWindow, sopen, sclose, lensr, focald, film)
{
	this->original = orginal;
	// Compute differential changes in origin for perspective camera rays
	dxCamera = RasterToCamera(Point(1, 0, 0)) - RasterToCamera(Point(0, 0, 0));
	dyCamera = RasterToCamera(Point(0, 1, 0)) - RasterToCamera(Point(0, 0, 0));

	// [tag]
	this->fov = fov;
}

//montecarlo.cpp
void ConcentricSampleDisk(Float u1, Float u2, Float *dx, Float *dy) {
	Float r, theta;
	// Map uniform random numbers to $[-1,1]^2$
	Float sx = 2 * u1 - 1;
	Float sy = 2 * u2 - 1;

	// Map square to $(r,\theta)$

	// Handle degeneracy at the origin
	if (sx == 0.0 && sy == 0.0) {
		*dx = 0.0;
		*dy = 0.0;
		return;
	}
	if (sx >= -sy) {
		if (sx > sy) {
			// Handle first region of disk
			r = sx;
			if (sy > 0.0) theta = sy / r;
			else          theta = 8.0f + sy / r;
		}
		else {
			// Handle second region of disk
			r = sy;
			theta = 2.0f - sx / r;
		}
	}
	else {
		if (sx <= sy) {
			// Handle third region of disk
			r = -sx;
			theta = 4.0f - sy / r;
		}
		else {
			// Handle fourth region of disk
			r = -sy;
			theta = 6.0f + sx / r;
		}
	}
	theta *= (Float)(M_PI / 4.f);
	*dx = r * cosf(theta);
	*dy = r * sinf(theta);
}

Float PerspectiveCamera::GenerateRay(const CameraSample &sample,
	Ray *ray) const {
	// Generate raster and camera samples
	//Point Pras(sample.imageX, sample.imageY, 0);
	//Point Pcamera;
	//RasterToCamera(Pras, &Pcamera);
	//*ray = Ray(Point(0, 0, 0), Normalize(Vector(Pcamera)), 0.f, INFINITY);
	//// Modify ray for depth of field
	//if (lensRadius > 0.) {
	//	// Sample point on lens
	//	Float lensU, lensV;
	//	ConcentricSampleDisk(sample.lensU, sample.lensV, &lensU, &lensV);
	//	lensU *= lensRadius;
	//	lensV *= lensRadius;

	//	// Compute point on plane of focus
	//	Float ft = focalDistance / ray->d.z;
	//	Point Pfocus = (*ray)(ft);

	//	// Update ray for effect of lens
	//	ray->o = Point(lensU, lensV, 0.f);
	//	ray->d = Normalize(Pfocus - ray->o);
	//}
	//ray->time = sample.time;
	const Float xyrate = 1.f;// = (Float)xResolution / (Float)yResolution;

	Float x = (sample.imageX - (Float)xResolution / 2.f) / ((Float)xResolution / 2.f);
	x *= tanf(Radians(fov / 2.f))*xyrate;
	Float y = (sample.imageY - (Float)yResolution / 2.f) / ((Float)yResolution / 2.f);
	y *= tanf(Radians(fov / 2.f));
	Float z = -1.f;
	const static Float rotate = Radians(-25.f);

	Float ry = y*cosf(rotate) - z*sinf(rotate);
	Float rz = y*sinf(rotate) + z*cosf(rotate);

	ray->d = Normalize(Vector(x, ry, rz));
	for (int i = 0; i < 3; ++i) {
		if (ray->d[i] == 0)
			TRACE_BUG("ray->d[%d]: %f\n", i, ray->d[i]);
		ray->dRcp[i] = (Float)1 / ray->d[i];
	}
	ray->o = original;
	//CameraToWorld(*ray, *ray);
	return 1.f;
}


Float PerspectiveCamera::GenerateRayDifferential(const CameraSample &sample,
	RayDifferential *ray) const {
	// Generate raster and camera samples
	Point Pras(sample.imageX, sample.imageY, 0);
	Point Pcamera;
	RasterToCamera(Pras, &Pcamera);
	Vector dir = Normalize(Vector(Pcamera.x, Pcamera.y, Pcamera.z));
	*ray = RayDifferential(Point(0, 0, 0), dir, 0.f, INFINITY);
	// Modify ray for depth of field
	if (lensRadius > 0.) {
		// Sample point on lens
		Float lensU, lensV;
		ConcentricSampleDisk(sample.lensU, sample.lensV, &lensU, &lensV);
		lensU *= lensRadius;
		lensV *= lensRadius;

		// Compute point on plane of focus
		Float ft = focalDistance / ray->d.z;
		Point Pfocus = (*ray)(ft);

		// Update ray for effect of lens
		ray->o = Point(lensU, lensV, 0.f);
		ray->d = Normalize(Pfocus - ray->o);
	}

	// Compute offset rays for _PerspectiveCamera_ ray differentials
	ray->rxOrigin = ray->ryOrigin = ray->o;
	ray->rxDirection = Normalize(Vector(Pcamera) + dxCamera);
	ray->ryDirection = Normalize(Vector(Pcamera) + dyCamera);
	ray->time = sample.time;
	CameraToWorld(*ray, *ray);
	ray->hasDifferentials = true;
	return 1.f;
}


void swap(Float& f1, Float& f2) {
	Float tmpf = f1;
	f1 = f2;
	f2 = tmpf;
}

PerspectiveCamera *CreatePerspectiveCamera(
	const AnimatedTransform &cam2world, Film *film, const Float &fov, const Point& original) {
	// Extract common camera parameters from _ParamSet_
	Float shutteropen = 0.f;
	Float shutterclose = 1.f;
	if (shutterclose < shutteropen) {
		TRACE_BUG("Shutter close time [%f] < shutter open [%f].  Swapping them.\n",
			shutterclose, shutteropen);
		swap(shutterclose, shutteropen);
	}
	Float lensradius = 0.f;
	Float focaldistance = 1e30f;
	Float frame = (Float)film->xResolution / (Float)film->yResolution;
	Float screen[4];
	if (frame > 1.f) {
		screen[0] = -frame;
		screen[1] = frame;
		screen[2] = -1.f;
		screen[3] = 1.f;
	}
	else {
		screen[0] = -1.f;
		screen[1] = 1.f;
		screen[2] = -1.f / frame;
		screen[3] = 1.f / frame;
	}
	//Float fov = 90.f;
	//Float halffov = fov/2;

	return new PerspectiveCamera(cam2world, screen, shutteropen,
		shutterclose, lensradius, focaldistance, fov, film, original);
}

/**
* 从ParamSet中获得camera信息，并创建camera
*/
//PerspectiveCamera *CreatePerspectiveCamera(const ParamSet &params,
//	const AnimatedTransform &cam2world, Film *film) {
//	// Extract common camera parameters from _ParamSet_
//	Float shutteropen = params.FindOneFloat("shutteropen", 0.f);
//	Float shutterclose = params.FindOneFloat("shutterclose", 1.f);
//	if (shutterclose < shutteropen) {
//		TRACE_BUG("Shutter close time [%f] < shutter open [%f].  Swapping them.",
//			shutterclose, shutteropen);
//		swap(shutterclose, shutteropen);
//	}
//	Float lensradius = params.FindOneFloat("lensradius", 0.f);
//	Float focaldistance = params.FindOneFloat("focaldistance", 1e30f);
//	Float frame = params.FindOneFloat("frameaspectratio",
//		Float(->xResolution) / Float(film->yResolution));
//	Float screen[4];
//	if (frame > 1.f) {
//		screen[0] = -frame;
//		screen[1] = frame;
//		screen[2] = -1.f;
//		screen[3] = 1.f;
//	}
//	else {
//		screen[0] = -1.f;
//		screen[1] = 1.f;
//		screen[2] = -1.f / frame;
//		screen[3] = 1.f / frame;
//	}
//	int swi;
//	const Float *sw = params.FindFloat("screenwindow", &swi);
//	if (sw && swi == 4)
//		memcpy(screen, sw, 4 * sizeof(Float));
//	Float fov = params.FindOneFloat("fov", 90.);
//	Float halffov = params.FindOneFloat("halffov", -1.f);
//	if (halffov > 0.f)
//		// hack for structure synth, which exports half of the full fov
//		fov = 2.f * halffov;
//	return new PerspectiveCamera(cam2world, screen, shutteropen,
//		shutterclose, lensradius, focaldistance, fov, film);
//}