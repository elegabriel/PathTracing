#include "film.h"
#include "../core/spectrum.h"
#include "../core/sampler.h"
#include "../core/point.h"

Film::Film(const int &xres, const int &yres, const bool &isFilter, const uint32_t radius)
	: xResolution(xres), yResolution(yres), is_filter(isFilter), guass_weight(NULL),
	m_radius(radius)
{
	m_bitmap.resize(xResolution*yResolution,Spectrum(0.f));
	m_weight.resize(xResolution*yResolution, 0.f);

	guass_weight = new Float[radius*radius];

	if (radius <= 0)
		m_radius = 2;
	else if (radius > 5)
		m_radius = 5;

	Float weight_sum = 0.0f;
	// 2*theta*theta = 2*(3*m_radius)*(3*m_radius)
	Float e = -1.0f / (Float)(12.5f * m_radius* m_radius);
	for (size_t y = 0; y < m_radius; y++)
		for (size_t x = 0; x < m_radius; x++) {
			guass_weight[m_radius * y + x] = exp((x*x + y*y)*e);
			weight_sum += guass_weight[m_radius * y + x];
		}

	for (size_t i = 0; i < m_radius*m_radius; i++) {
		guass_weight[i] /= weight_sum;
	}
}

Film::~Film()
{
	if (guass_weight)
		delete[] guass_weight;
}

void Film::GetSampleExtent(int * xstart, int * xend, int * ystart, int * yend) const
{
	*xstart = (Int)floorf(0.f + 0.5f - (Float)m_radius);
	*xend = (Int)floorf(0.f + 0.5f + (Float)xResolution  + (Float)m_radius);
	*ystart = (Int)floorf(0.f + 0.5f - (Float)m_radius);
	*yend = (Int)floorf(0.f + 0.5f + (Float)yResolution + (Float)m_radius);

	/**xstart = (int)(xPixelStart + 0.5f - filter->xWidth);
	*xend = Ceil2Int(xPixelStart + 0.5f + xPixelCount +
		filter->xWidth);
	*ystart = Floor2Int(yPixelStart + 0.5f - filter->yWidth);
	*yend = Ceil2Int(yPixelStart + 0.5f + yPixelCount +
		filter->yWidth);*/
}

void Film::setPixel(const Point3 &pos, const Spectrum &spec)
{
	if (!is_filter)
	{
		Int x =(Int) pos.x, y = (Int) pos.y;
		m_bitmap[y*xResolution + x] += spec;
		m_weight[y*xResolution + x] += 1.0f;
	}
	/*filter the image*/
	else {
		{
			Int minX = std::max((int)std::ceil(pos.x - m_radius), 0),
				minY = std::max((int)std::ceil(pos.y - m_radius), 0),
				maxX = std::min((int)std::floor(pos.x + m_radius), xResolution - 1),
				maxY = std::min((int)std::floor(pos.y + m_radius), yResolution - 1);

			for (int y = minY; y < maxY; y++)
			{
				for (int x = minX; x < maxX; x++) {
					Float weight = guass_weight[int(abs(y - pos.y)) * m_radius + int(abs(x - pos.x))];
					int index = y*xResolution + x;
					m_bitmap[index] += spec*weight;
					m_weight[index] += weight;
				}
			}
		}
	}
}

Int limit(const Int num, const Int limit1, const Int limit2) {
	if (num >= limit1 && num <= limit2) return num;
	if (num < limit1)
		return limit1;
	else
		return limit2;
}

inline double clamp(Float x) { return x<0 ? 0 : x>1 ? 1 : x; }
inline int toInt(Float x) { return int(pow(clamp(x), 1 / 2.2) * 255 + .5); }

void Film::saveImage(const std::string & filename)
{
	FIBITMAP *image = FreeImage_Allocate(xResolution, yResolution, 32);
	int pitch = FreeImage_GetPitch(image);
	int bpp = FreeImage_GetBPP(image);
	BYTE* pDst = FreeImage_GetBits(image);
	BYTE* img = new BYTE[4*xResolution*yResolution];
	for (size_t i = 0; i < (uint32_t)(xResolution*yResolution); i++)
	{
		img[i * 4 + 0] = toInt(m_bitmap[i][0]);
		img[i * 4 + 1] = toInt(m_bitmap[i][1]);
		img[i * 4 + 2] = toInt(m_bitmap[i][2]);
		img[i * 4 + 3] = 255;
	}

	memcpy(pDst, &img[0], pitch*yResolution);
	auto fullname = filename + ".png";
	if (FreeImage_Save(FIF_PNG, image, fullname.c_str()))
		printf("\nImage %s saved successfully\n\n", fullname.c_str());
	else
		printf("\n***IMAGE NOT SAVED***\n\n");

	delete[]img;
}
