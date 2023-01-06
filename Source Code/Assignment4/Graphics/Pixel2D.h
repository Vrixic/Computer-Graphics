#pragma once
#include "Math/IntPoint2D.h"

struct Pixel2D : IntPoint2D
{
public:
	unsigned int Color;

public:
	inline Pixel2D(int x, int y, unsigned int color = 0);

	inline Pixel2D(IntPoint2D point, unsigned int color = 0);
};

inline Pixel2D::Pixel2D(int x, int y, unsigned int color)
	: IntPoint2D(x, y), Color(color) { }

inline Pixel2D::Pixel2D(IntPoint2D point, unsigned int color)
	: IntPoint2D(point.X, point.Y), Color(color) { }
