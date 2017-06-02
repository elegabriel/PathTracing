#include "../render/film.h"
#include "ray.h"
#include "transform.h"
#include "sampler.h"

#ifndef __CAMERA_H__
#define __CAMERA_H__

class Camera {
public:
	// Camera Interface
	Camera(const AnimatedTransform &cam2world, Float sopen, Float sclose,
		Film *film);
	virtual ~Camera();
	virtual Float GenerateRay(const CameraSample &sample, Ray *ray) const = 0;
	virtual Float GenerateRayDifferential(const CameraSample &sample, RayDifferential *rd) const;
	// [tag]
	const AnimatedTransform& getCameraToWorld()const { return CameraToWorld; }
	// Camera Public Data
	AnimatedTransform CameraToWorld;
	const Float shutterOpen, shutterClose;
	uint32_t xResolution, yResolution;
	Film *film;
};

class ProjectiveCamera : public Camera {
public:
	// ProjectiveCamera Public Methods
	ProjectiveCamera(const AnimatedTransform &cam2world,
		//screenWindows����Ļ��������x���򳤶ȴ���y������ȡ[0]��[1]Ϊx/y��-x/y��ȡ[2]��[3]Ϊ-1��1����֮��Ȼ
		const Transform &proj, const Float screenWindow[4],
		Float sopen, Float sclose, Float lensr, Float focald, Film *film);
	Point original;
protected:
	// ProjectiveCamera Protected Data
	Transform CameraToScreen, RasterToCamera;
	Transform ScreenToRaster, RasterToScreen;
	Float lensRadius, focalDistance;
	
};

class PerspectiveCamera : public ProjectiveCamera {
public:
	// PerspectiveCamera Public Methods
	PerspectiveCamera(const AnimatedTransform &cam2world,
		const Float screenWindow[4], Float sopen, Float sclose,
		Float lensr, Float focald, Float fov, Film *film, const Point &orginal);
	Float GenerateRay(const CameraSample &sample, Ray *) const;
	Float GenerateRayDifferential(const CameraSample &sample,
		RayDifferential *ray) const;
	Float getFov()const { return fov; }
private:
	// PerspectiveCamera Private Data
	Vector dxCamera, dyCamera;
	// [tag] �ӽ�
	Float fov;
};

PerspectiveCamera *CreatePerspectiveCamera(
	const AnimatedTransform &cam2world, Film *film, const Float &fov,const Point& original);

//PerspectiveCamera *CreatePerspectiveCamera(const ParamSet &params,
//	const AnimatedTransform &cam2world, Film *film);

#endif // !__CAMERA_H__
