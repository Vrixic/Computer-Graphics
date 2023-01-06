#pragma once
#include "Math/Math.h"

class Camera
{
private:
	Vector3D mTranslationVector;
	Vector3D mRotationVector;

	Matrix4D mViewMatrix;

public:
	Camera();

public:
	void Translate(float x, float y, float z);

	void Rotate(float yaw, float pitch, float roll);

	void SetViewMatrix();

	Vector3D GetForwardVector();

	Matrix4D& GetViewMatrix();
};

