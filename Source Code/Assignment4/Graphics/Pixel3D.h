#pragma once
#include "Pixel2D.h"

struct Pixel3D : Pixel2D
{
public:
	float Z;

public:
	inline Pixel3D(int x, int y, float z = 0.0f, unsigned int color = 0);

	inline Pixel3D(IntPoint2D point, float z = 0.0f, unsigned int color = 0);

	inline Pixel3D(Pixel2D point, float z = 0.0f);
};

inline Pixel3D::Pixel3D(int x, int y, float z, unsigned int color)
	: Pixel2D(x, y, color), Z(z) { }

inline Pixel3D::Pixel3D(IntPoint2D point, float z, unsigned int color)
	: Pixel2D(point, color), Z(z) { }

inline Pixel3D::Pixel3D(Pixel2D point, float z)
	: Pixel2D(point.X, point.Y, point.Color), Z(z) { }