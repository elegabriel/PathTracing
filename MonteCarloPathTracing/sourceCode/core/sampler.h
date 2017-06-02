#include "../define.h"
#include "rng.h"

#ifndef __SAMPLER_H__
#define __SAMPLER_H__

class Sampler {
public:
	Sampler(int xstart, int xend, int ystart, int yend, int spp, Float sopen, 
		Float sclose):
		xPixelStart(xstart), xPixelEnd(xend), yPixelStart(ystart), yPixelEnd(yend), 
		samplesPerPixel(spp), shutterOpen(sopen), shutterClose(sclose) { };

	virtual Int GetMoreSamples(Sample *samples) = 0;
	//virtual Sampler *GetSubSampler(int num, int count) = 0;
	
	const int xPixelStart, xPixelEnd, yPixelStart, yPixelEnd;
	const int samplesPerPixel;
	const float shutterOpen, shutterClose;

protected:
	void ComputeSubWindow(int num, int count, int *xstart, int *xend, int *ystart, int *yend) const;
private:

};


class HaltonSampler : public Sampler {
public:
	HaltonSampler(int xs, int xe, int ys, int ye, int ps, float sopen, float sclose);
	int MaximumSampleCount() { return 1; }
	//Sampler *GetSubSampler(int num, int count);

	virtual Int GetMoreSamples(Sample *samples);
	
private:
	int wantedSamples, currentSample;
};

struct CameraSample {
public:
	Float imageX, imageY;
	Float lensU, lensV;
	Float time;
};

struct Sample :public CameraSample {
public:
	Sample() {};
};

HaltonSampler *CreateHaltonSampler(const Film * film, const Camera * camera, 
	const int samplesPerPixel);

#endif // !__SAMPLER_H__
