#pragma once

#include "Defines.h"
#include <math.h>
#include "Vector4D.h"

struct Matrix4D
{
private:
	float M[4][4];

public:
	inline Matrix4D();

	inline Matrix4D(const Vector4D& a, const Vector4D& b, const Vector4D& c, const Vector4D& d);

	inline Matrix4D(float n00, float n01, float n02, float n03,
		float n10, float n11, float n12, float n13,
		float n20, float n21, float n22, float n23,
		float n30, float n31, float n32, float n33);

public:
	inline float& operator()(int i, int j)
	{
		return (M[i][j]);
	}

	inline const float& operator()(int i, int j) const
	{
		return (M[i][j]);
	}

	/* Returns row 0 - 2*/
	inline Vector4D& operator[](int i)
	{
		return (*reinterpret_cast<Vector4D*>(M[i]));
	}

	inline const Vector4D& operator[](int i) const
	{
		return (*reinterpret_cast<const Vector4D*>(M[i]));
	}
	/* row vector multiply by matrix */
	inline Vector4D operator*(const Vector4D& v) const;

	inline Matrix4D operator*(const Matrix4D& otherM) const;

public:

	inline static Matrix4D Identity();

	inline void SetIdentity();

	inline void SetTranslation(const Vector3D& translation);

	// Translate Matrix 
	inline void TranslateMatrix(const Vector3D& translation);

	/* Euler Angles Rotation Calculation */

	// Rotation over X - axis, returns matrix 
	inline static Matrix4D CreateRotX(float angle);

	// Rotation over Y - axis, returns matrix
	inline static Matrix4D CreateRotY(float angle);

	// Rotation over Z - axis, return matrix
	inline static Matrix4D CreateRotZ(float angle);

	// Scales the matrix
	inline void ScaleMatrix(const Vector3D& scale);
};

inline Matrix4D::Matrix4D()
	: Matrix4D(0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f) {}

inline Matrix4D::Matrix4D(float n00, float n01, float n02, float n03,
	float n10, float n11, float n12, float n13,
	float n20, float n21, float n22, float n23,
	float n30, float n31, float n32, float n33)
{
	M[0][0] = n00;
	M[0][1] = n01;
	M[0][2] = n02;
	M[0][3] = n03;

	M[1][0] = n10;
	M[1][1] = n11;
	M[1][2] = n12;
	M[1][3] = n13;

	M[2][0] = n20;
	M[2][1] = n21;
	M[2][2] = n22;
	M[2][3] = n23;

	M[3][0] = n30;
	M[3][1] = n31;
	M[3][2] = n32;
	M[3][3] = n33;
}

inline Matrix4D::Matrix4D(const Vector4D& a, const Vector4D& b, const Vector4D& c, const Vector4D& d)
{
	M[0][0] = a.X;
	M[0][1] = a.Y;
	M[0][2] = a.Z;
	M[0][3] = a.W;

	M[1][0] = b.X;
	M[1][1] = b.Y;
	M[1][2] = b.Z;
	M[1][3] = b.W;

	M[2][0] = c.X;
	M[2][1] = c.Y;
	M[2][2] = c.Z;
	M[2][3] = c.W;

	M[3][0] = d.X;
	M[3][1] = d.Y;
	M[3][2] = d.Z;
	M[3][3] = d.W;
}

inline Vector4D Matrix4D::operator*(const Vector4D& v) const
{
	return Vector4D
	(
		(v.X * M[0][0] + v.Y * M[1][0] + v.Z * M[2][0] + v.W * M[3][0]),
		(v.X * M[0][1] + v.Y * M[1][1] + v.Z * M[2][1] + v.W * M[3][1]),
		(v.X * M[0][2] + v.Y * M[1][2] + v.Z * M[2][2] + v.W * M[3][2]),
		(v.X * M[0][3] + v.Y * M[1][3] + v.Z * M[2][3] + v.W * M[3][3])
	);
}

inline Matrix4D Matrix4D::operator*(const Matrix4D& otherM) const
{
	return Matrix4D
	(
		(M[0][0] * otherM(0, 0) + M[0][1] * otherM(1, 0) + M[0][2] * otherM(2, 0) + M[0][3] * otherM(3, 0)),
		(M[0][0] * otherM(0, 1) + M[0][1] * otherM(1, 1) + M[0][2] * otherM(2, 1) + M[0][3] * otherM(3, 1)),
		(M[0][0] * otherM(0, 2) + M[0][1] * otherM(1, 2) + M[0][2] * otherM(2, 2) + M[0][3] * otherM(3, 2)),
		(M[0][0] * otherM(0, 3) + M[0][1] * otherM(1, 3) + M[0][2] * otherM(2, 3) + M[0][3] * otherM(3, 3)),

		(M[1][0] * otherM(0, 0) + M[1][1] * otherM(1, 0) + M[1][2] * otherM(2, 0) + M[1][3] * otherM(3, 0)),
		(M[1][0] * otherM(0, 1) + M[1][1] * otherM(1, 1) + M[1][2] * otherM(2, 1) + M[1][3] * otherM(3, 1)),
		(M[1][0] * otherM(0, 2) + M[1][1] * otherM(1, 2) + M[1][2] * otherM(2, 2) + M[1][3] * otherM(3, 2)),
		(M[1][0] * otherM(0, 3) + M[1][1] * otherM(1, 3) + M[1][2] * otherM(2, 3) + M[1][3] * otherM(3, 3)),

		(M[2][0] * otherM(0, 0) + M[2][1] * otherM(1, 0) + M[2][2] * otherM(2, 0) + M[2][3] * otherM(3, 0)),
		(M[2][0] * otherM(0, 1) + M[2][1] * otherM(1, 1) + M[2][2] * otherM(2, 1) + M[2][3] * otherM(3, 1)),
		(M[2][0] * otherM(0, 2) + M[2][1] * otherM(1, 2) + M[2][2] * otherM(2, 2) + M[2][3] * otherM(3, 2)),
		(M[2][0] * otherM(0, 3) + M[2][1] * otherM(1, 3) + M[2][2] * otherM(2, 3) + M[2][3] * otherM(3, 3)),

		(M[3][0] * otherM(0, 0) + M[3][1] * otherM(1, 0) + M[3][2] * otherM(2, 0) + M[3][3] * otherM(3, 0)),
		(M[3][0] * otherM(0, 1) + M[3][1] * otherM(1, 1) + M[3][2] * otherM(2, 1) + M[3][3] * otherM(3, 1)),
		(M[3][0] * otherM(0, 2) + M[3][1] * otherM(1, 2) + M[3][2] * otherM(2, 2) + M[3][3] * otherM(3, 2)),
		(M[3][0] * otherM(0, 3) + M[3][1] * otherM(1, 3) + M[3][2] * otherM(2, 3) + M[3][3] * otherM(3, 3))
	);
}

inline Matrix4D Matrix4D::Identity()
{
	return Matrix4D
	(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

inline void Matrix4D::SetIdentity()
{
	M[0][0] = 1.0f;
	M[0][1] = 0.0f;
	M[0][2] = 0.0f;
	M[0][3] = 0.0f;

	M[1][0] = 0.0f;
	M[1][1] = 1.0f;
	M[1][2] = 0.0f;
	M[1][3] = 0.0f;

	M[2][0] = 0.0f;
	M[2][1] = 0.0f;
	M[2][2] = 1.0f;
	M[2][3] = 0.0f;

	M[3][0] = 0.0f;
	M[3][1] = 0.0f;
	M[3][2] = 0.0f;
	M[3][3] = 1.0f;
}

inline void Matrix4D::SetTranslation(const Vector3D& translation)
{
	M[3][0] = translation.X;
	M[3][1] = translation.Y;
	M[3][2] = translation.Z;
}

inline void Matrix4D::TranslateMatrix(const Vector3D& translation)
{
	M[3][0] += translation.X;
	M[3][1] += translation.Y;
	M[3][2] += translation.Z;
}

inline void Matrix4D::ScaleMatrix(const Vector3D& scale)
{
	M[0][0] += scale.X;
	M[1][1] += scale.Y;
	M[2][2] += scale.Z;
}

inline Matrix4D Matrix4D::CreateRotX(float angle)
{
	float c = static_cast<float>(cos((PI / 180) * angle));
	float s = static_cast<float>(sin((PI / 180) * angle));

	return Matrix4D
	(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, c, -s, 0.0f,
		0.0f, s, c, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

inline Matrix4D Matrix4D::CreateRotY(float angle)
{
	float c = static_cast<float>(cos((PI / 180) * angle));
	float s = static_cast<float>(sin((PI / 180) * angle));

	return Matrix4D
	(
		c, 0.0f, s, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		-s, 0.0f, c, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

inline Matrix4D Matrix4D::CreateRotZ(float angle)
{
	float c = static_cast<float>(cos((PI / 180.0f) * angle));
	float s = static_cast<float>(sin((PI / 180.0f) * angle));

	return Matrix4D
	(
		c, -s, 0.0f, 0.0f,
		s, c, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}
