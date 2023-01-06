#pragma once

#include "Math/Math.h"
#include <vector>

enum class TextureFilter
{
	NEAREST,
	BILINEAR,
	TRILINEAR
};

// Vertex Shader
void (*VERTEX_SHADER)(Vertex&) = 0;

// Pixel Shader
void (*PIXEL_SHADER)(unsigned int&) = 0;

// Shader variables 'SV_'
Matrix4D SV_WorldMatrix;
Matrix4D SV_ViewMatrix;
Matrix4D SV_ProjectionMatrix;

Texture SV_Texture;
unsigned int SV_MaxMipMapLevel;
unsigned int SV_MipMapLevel;

float SV_TexCoordU;
float SV_TexCoordV;

TextureFilter SV_TextureFilter;

/* To Be Changed */
float SV_NearPlane;
float SV_FarPlane;

/* Lighting */
Vector3D SV_DirectionLightDirection;
unsigned int SV_DirectionalLightColor;

Vector3D SV_PointLightPosition;
unsigned int SV_PointLightColor;
float SV_PointLightRadius;

float SV_AmbientTerm;

std::vector<Texture*> Textures;

void VS_World(Vertex& vertex)
{
	Math::MultiplyVertexByMatrix(vertex, SV_WorldMatrix);
	Math::MultiplyVertexByMatrix(vertex, SV_ViewMatrix);
	Math::MultiplyVertexByMatrix(vertex, SV_ProjectionMatrix);

	float lightRatio = Math::Clamp(0.0f, 1.0f, Vector3D::DotProduct(-SV_DirectionLightDirection, vertex.Normal));
	vertex.Color = Math::LerpColor(0, SV_DirectionalLightColor, lightRatio);

	/* Point light */
	Vector3D lightSurfaceDisplacement = SV_PointLightPosition - Vector3D(vertex.X, vertex.Y, vertex.Z);
	lightRatio = Math::Clamp(0.0f, 1.0f, Vector3D::DotProduct(lightSurfaceDisplacement, vertex.Normal));
	lightRatio = Math::Clamp(0.0f, 1.0f, lightRatio + SV_AmbientTerm);
	float attenuation = 1.0f - Math::Clamp(0.0f, 1.0f, lightSurfaceDisplacement.Length() / SV_PointLightRadius);
	vertex.Color = Math::LerpColor(vertex.Color, SV_PointLightColor, attenuation * lightRatio);
}

void PS_RedColor(unsigned int& pixel)
{
	pixel = RED;
}

void PS_GreenColor(unsigned int& pixel)
{
	pixel = GREEN;
}

void PS_WhiteColor(unsigned int& pixel)
{
	pixel = 0xffffffff;
}

void PS_BlueColor(unsigned int& pixel)
{
	pixel = BLUE;
}

void PS_NoColor(unsigned int& pixel) { }

void PS_Texture(unsigned int& pixel)
{
	SV_Texture = *Textures[static_cast<unsigned int>(SV_MipMapLevel)];

	float uRatio = SV_TexCoordU * SV_Texture.Width;
	float vRatio = SV_TexCoordV * SV_Texture.Height;

	int x = static_cast<int>(uRatio);
	int y = static_cast<int>(vRatio);

	unsigned int currentTexel = Math::Convert2DTo1D(x, y, SV_Texture.Width);
	pixel = (SV_Texture.Pixels[currentTexel]);

	switch (SV_TextureFilter)
	{

	case TextureFilter::BILINEAR:
	{
		uRatio -= x;
		vRatio -= y;

		pixel = Math::CalculateBilinearTextureFilter(uRatio, vRatio, currentTexel, SV_Texture);
		break;
	}

	case TextureFilter::TRILINEAR:
	{
		// Bi-Linear
		uRatio -= x;
		vRatio -= y;

		unsigned int color1 = Math::CalculateBilinearTextureFilter(uRatio, vRatio, currentTexel, SV_Texture);

		// Trilinear 
		unsigned int mipMapLevelCeil = static_cast<unsigned int>(ceil(SV_MipMapLevel));
		mipMapLevelCeil = mipMapLevelCeil > SV_MaxMipMapLevel ? SV_MaxMipMapLevel : mipMapLevelCeil;

		float mipMapLevelCeilRatio = static_cast<float>(mipMapLevelCeil - SV_MipMapLevel);
		Texture& trilinearTexture = *Textures[mipMapLevelCeil];

		uRatio = SV_TexCoordU * trilinearTexture.Width;
		vRatio = SV_TexCoordV * trilinearTexture.Height;

		x = static_cast<int>(uRatio);
		y = static_cast<int>(vRatio);

		currentTexel = Math::Convert2DTo1D(x, y, trilinearTexture.Width);
		pixel = (trilinearTexture.Pixels[currentTexel]);

		uRatio -= x;
		vRatio -= y;
		
		unsigned int color2 = Math::CalculateBilinearTextureFilter(uRatio, vRatio, currentTexel, trilinearTexture);

		pixel = Math::LerpColor(color1, color2, mipMapLevelCeilRatio);
		break;
	}

	}
}