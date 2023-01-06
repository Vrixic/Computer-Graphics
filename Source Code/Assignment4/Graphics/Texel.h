#pragma once
#include "Pixel3D.h"

struct Texel : Pixel3D
{
public:
	// Linear Z
	float W;

	// Texture Coord
	float TexCoordU;
	float TexCoordV;

public:
	inline Texel(int x, int y, float z = 0.0f, float w = 0.0f, float texCoordU = -1.0f, float texCoordV = -1.0f, unsigned int color = 0);

	inline Texel(IntPoint2D point, float z = 0.0f, float w = 0.0f, float texCoordU = -1.0f, float texCoordV = -1.0f, unsigned int color = 0);

	inline Texel(Pixel3D point, float w = 0.0f, float texCoordU = -1.0f, float texCoordV = -1.0f);
};

inline Texel::Texel(int x, int y, float z, float w, float texCoordU, float texCoordV, unsigned int color)
	: Pixel3D(x, y, z, color), W(w),  TexCoordU(texCoordU), TexCoordV(texCoordV) { }

inline Texel::Texel(IntPoint2D point, float z, float w, float texCoordU, float texCoordV, unsigned int color)
	: Pixel3D(point, z, color), W(w), TexCoordU(texCoordU), TexCoordV(texCoordV) { }

inline Texel::Texel(Pixel3D point, float w, float texCoordU, float texCoordV)
	: Pixel3D(point.X, point.Y, point.Z, point.Color), W(w), TexCoordU(texCoordU), TexCoordV(texCoordV) { }