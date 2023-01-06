#include "Camera.h"

Camera::Camera()
{

}

void Camera::Translate(float x, float y, float z)
{
	mTranslationVector.X += x;
	mTranslationVector.Y += y;
	mTranslationVector.Z += z;
}

void Camera::Rotate(float yaw, float pitch, float roll)
{
	mRotationVector.X += pitch;
	mRotationVector.Y += yaw;
	mRotationVector.Z += roll;
}

void Camera::SetViewMatrix()
{
	Matrix4D translationMatrix;
	translationMatrix.SetIdentity();
	translationMatrix.TranslateMatrix(mTranslationVector);

	mViewMatrix = (Matrix4D::CreateRotX(mRotationVector.X) * Matrix4D::CreateRotY(mRotationVector.Y) * Matrix4D::CreateRotZ(mRotationVector.Z));
	mViewMatrix = (translationMatrix * mViewMatrix);
	mViewMatrix = Math::OrthogonalInverseMatrix(mViewMatrix);
}

Vector3D Camera::GetForwardVector()
{
	Vector4D& v = mViewMatrix[3];
	return Vector3D(v.X, v.Y, v.Z);
}

Matrix4D& Camera::GetViewMatrix()
{
	return mViewMatrix;
}
