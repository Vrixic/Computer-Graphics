#pragma once
#include "Math/Vector4D.h"

struct Vertex : Vector4D
{
public:
	/* Normals */
	Vector3D Normal;

	/* UV Texture Coordinates for Texture Mapping*/
	float TexCoordU;
	float TexCoordV;

	unsigned int Color;

public:
	inline Vertex();

	inline Vertex(float scalar, unsigned int color = 0);

	inline Vertex(float x, float y, float z, float w = 1, float texCoordU = -1.0f, float texCoordV = -1.0f, unsigned int color = 0);

	inline Vertex(const Vector3D& v, float w = 1, float texCoordU = -1.0f, float texCoordV = -1.0f, unsigned int color = 0);

	inline Vertex(const Vector4D& v, float texCoordU = -1.0f, float texCoordV = -1.0f, unsigned int color = 0);

public:
	inline Vertex operator+(const Vector3D& v) const;

public:
	inline void PerspectiveDivide();

	inline void ScaleVector(float scalar);
};

inline Vertex::Vertex()
	: Vertex(0.0f, 0) { }

inline Vertex::Vertex(float scalar, unsigned int color)
	: Vector4D(scalar), TexCoordU(-1), TexCoordV(-1), Color(color) {}

inline Vertex::Vertex(float x, float y, float z, float w, float texCoordU, float texCoordV, unsigned int color)
	: Vector4D(x, y, z, w), TexCoordU(texCoordU), TexCoordV(texCoordV), Color(color) {}

inline Vertex::Vertex(const Vector3D& v, float w, float texCoordU, float texCoordV, unsigned int color)
	: Vector4D(v, w), TexCoordU(texCoordU), TexCoordV(texCoordV), Color(color) {}

inline Vertex::Vertex(const Vector4D& v, float texCoordU, float texCoordV, unsigned int color)
	: Vector4D(v.X, v.Y, v.Z, v.W), TexCoordU(texCoordU), TexCoordV(texCoordV), Color(color) {}

inline Vertex Vertex::operator+(const Vector3D& v) const
{
	return Vertex(X + v.X, Y + v.Y, Z + v.Z, W, TexCoordU, TexCoordV, Color);
}

inline void Vertex::PerspectiveDivide()
{
	float r = 1.0f / W;
	X *= r;
	Y *= r;
	Z *= r;
}

inline void Vertex::ScaleVector(float scalar)
{
	X *= scalar;
	Y *= scalar;
	Z *= scalar;
}