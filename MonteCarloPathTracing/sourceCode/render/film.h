#include "../define.h"
#include "../../freeimage.h"

#ifndef __FILM_H__
#define __FILM_H__

class Film {
public:
	Film(const int &xres, const int &yres, const bool &isFilter = false,
		const uint32_t radius = 2
	);
	~Film();
	void GetSampleExtent(int *xstart, int *xend, int *ystart, int *yend) const;
	const int xResolution, yResolution;

	void setPixel(const Point3 &pos, const Spectrum &spec);
	void saveImage(const std::string& filename );
protected:
	std::vector<Spectrum> m_bitmap;
	std::vector<Float> m_weight;
	bool is_filter;
	uint32_t m_radius;
	Float* guass_weight;
};

#endif // !__FILM_H__
