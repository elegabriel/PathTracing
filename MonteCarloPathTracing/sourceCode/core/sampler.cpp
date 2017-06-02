#include "sampler.h"
#include "camera.h"
#include "../render/film.h"

HaltonSampler::HaltonSampler(int xs, int xe, int ys, int ye, int ps,
	float sopen, float sclose)
	: Sampler(xs, xe, ys, ye, ps, sopen, sclose) {
	int delta = std::max(xPixelEnd - xPixelStart,
		yPixelEnd - yPixelStart);
	wantedSamples = samplesPerPixel * delta * delta;
	currentSample = 0;
}

//Sampler * HaltonSampler::GetSubSampler(int num, int count)
//{
//	int x0, x1, y0, y1;
//	ComputeSubWindow(num, count, &x0, &x1, &y0, &y1);
//	if (x0 == x1 || y0 == y1) return NULL;
//	return new HaltonSampler(x0, x1, y0, y1, samplesPerPixel, shutterOpen,
//		shutterClose);
//}


Int HaltonSampler::GetMoreSamples(Sample * samples)
{
retry:
	if (currentSample >= wantedSamples) return 0;
	
	// Generate sample with Halton sequence and reject if outside image extent
	// Halton采样的主要部分，类似绳子取半
	Float u = RadicalInverse(currentSample, 3);
	Float v = RadicalInverse(currentSample, 2);
	Float lerpDelta = Float(std::max(xPixelEnd - xPixelStart,
		yPixelEnd - yPixelStart));
	samples->imageX = Lerp((Float)u, (Float)xPixelStart, (Float)(xPixelStart + lerpDelta));
	samples->imageY = Lerp((Float)v, (Float)yPixelStart, (Float)(yPixelStart + lerpDelta));
	++currentSample;
	if (samples->imageX >= xPixelEnd || samples->imageY >= yPixelEnd)
		goto retry;
	return 1;
}

HaltonSampler * CreateHaltonSampler(const Film * film, const Camera * camera, 
	const int samplesPerPixel )
{
	int xstart, xend, ystart, yend;
	film->GetSampleExtent(&xstart, &xend, &ystart, &yend);
	return new HaltonSampler(xstart, xend, ystart, yend, samplesPerPixel,
		camera->shutterOpen, camera->shutterClose);
}

//void Sampler::ComputeSubWindow(int num, int count, int *newXStart,
//	int *newXEnd, int *newYStart, int *newYEnd) const
//{
//	// Determine how many tiles to use in each dimension, _nx_ and _ny_
//	int dx = xPixelEnd - xPixelStart, dy = yPixelEnd - yPixelStart;
//	int nx = count, ny = 1;
//	while ((nx & 0x1) == 0 && 2 * dx * ny < dy * nx) {
//		nx >>= 1;
//		ny <<= 1;
//	}
//	assert(nx * ny == count);
//
//	// Compute $x$ and $y$ pixel sample range for sub-window
//	int xo = num % nx, yo = num / nx;
//	float tx0 = float(xo) / float(nx), tx1 = float(xo + 1) / float(nx);
//	float ty0 = float(yo) / float(ny), ty1 = float(yo + 1) / float(ny);
//	*newXStart = (int)floorf(Lerp(tx0, xPixelStart, xPixelEnd));
//	*newXEnd = (int)floorf(Lerp(tx1, xPixelStart, xPixelEnd));
//	*newYStart = (int)floorf(Lerp(ty0, yPixelStart, yPixelEnd));
//	*newYEnd = (int)floorf(Lerp(ty1, yPixelStart, yPixelEnd));
//}
