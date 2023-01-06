#include "Application.h"
#include "Rasterization_Functions.h"
#include "Camera.h"

#include "Textures/InnSigns/celestial.h"
#include "Textures/InnSigns/flower.h"
#include "Textures/InnSigns/greendragon.h"
#include "Textures/InnSigns/treeolife.h"

#include "StoneHenge.h"
#include "StoneHenge_Texture.h"

void GenerateMipMapForTexture(Texture& lastTexture)
{
	Texture* newTexture = new Texture();
	newTexture->Width = lastTexture.Width / 2;
	newTexture->Height = lastTexture.Height / 2;
	newTexture->NumOfPixels = newTexture->Width * newTexture->Height;

	unsigned int* pixels = new unsigned int[newTexture->NumOfPixels];

	for (unsigned int i = 0; i < lastTexture.Height; i += 2)
	{
		for (unsigned int j = 0; j < lastTexture.Width; j += 2)
		{
			unsigned int position = Math::Convert2DTo1D(j, i, lastTexture.Width);

			unsigned int nextPixel = (position + 1) % lastTexture.NumOfPixels;
			unsigned int bottomPixel = (position + lastTexture.Width) % lastTexture.NumOfPixels;
			unsigned int diagonalPixel = (bottomPixel + 1) % lastTexture.NumOfPixels;

			unsigned int color1 = Math::LerpColor(lastTexture.Pixels[position], (lastTexture.Pixels[nextPixel % lastTexture.NumOfPixels]), 0.5f);
			unsigned int color2 = Math::LerpColor((lastTexture.Pixels[bottomPixel % lastTexture.NumOfPixels]), (lastTexture.Pixels[diagonalPixel % lastTexture.NumOfPixels]), 0.5f);
			pixels[Math::Convert2DTo1D(j / 2, i / 2, newTexture->Width)] = Math::LerpColor(color1, color2, 0.5f);
		}
	}

	newTexture->Pixels = pixels;
	Textures.push_back(newTexture);

	if (newTexture->Width != 1 && newTexture->Height != 1)
	{
		GenerateMipMapForTexture(*newTexture);
	}
}

void InitializeStars()
{
	srand(time(NULL));

	Vertex starVertex;
	starVertex.W = 1.0f;
	starVertex.Color = WHITE;

	float randScalar = (1.0f / (RAND_MAX * 0.5f));
	for (int i = 0; i < STARS_COUNT; i++)
	{
		starVertex.X = ((rand() * randScalar) - 1.0f) * 50.0f;
		starVertex.Y = ((rand() * randScalar) - 1.0f) * 50.0f;
		starVertex.Z = ((rand() * randScalar) - 1.0f) * 50.0f;

		starsVertices[i] = starVertex;
	}
}

void InitializeStoneHedge()
{
	Vertex temp;
	temp.Color = WHITE;
	for (unsigned int i = 0; i < 1457; i++)
	{
		temp.X = StoneHenge_data[i].pos[0] * 0.1f;
		temp.Y = StoneHenge_data[i].pos[1] * 0.1f;
		temp.Z = StoneHenge_data[i].pos[2] * 0.1f;
		temp.W = 1.0f;

		temp.TexCoordU = StoneHenge_data[i].uvw[0];
		temp.TexCoordV = StoneHenge_data[i].uvw[1];

		temp.Normal.X = StoneHenge_data[i].nrm[0];
		temp.Normal.Y = StoneHenge_data[i].nrm[1];
		temp.Normal.Z = StoneHenge_data[i].nrm[2];

		stoneHedgeVertices[i] = temp;
	}
}

void Application::Init()
{
	RS_Initialize("Vrij Patel", RASTER_WIDTH, RASTER_HEIGHT);

	InitializeStars();
	InitializeStoneHedge();

	Update();
}

void Application::Update()
{
	VERTEX_SHADER = VS_World;
	PIXEL_SHADER = PS_RedColor;

	Camera worldCamera;
	worldCamera.Translate(0.0f, 0.0f, -1.0f);
	worldCamera.Rotate(0.0f, -18.0f, 0.0f);

	float worldCameraTranslateSpeed = 60.0f;

	SV_NearPlane = 0.1f;
	SV_FarPlane = 100.0f;

	Matrix4D projectionMatrix = Math::GetProjectionMatrix
	(
		RASTER_WIDTH, RASTER_HEIGHT,
		90.0f, SV_NearPlane, SV_FarPlane
	);

	float timePassed = 0.0f;

	SV_TextureFilter = TextureFilter::NEAREST;

	// Mip Map
	unsigned int numOFPixels = StoneHenge_width * StoneHenge_height;
	unsigned int* convertPixels = new unsigned int[numOFPixels];

	for (unsigned int i = 0; i < numOFPixels; i++)
	{
		convertPixels[i] = Math::ConvertBGRAToARGB(StoneHenge_pixels[i]);
	}

	Texture* stoneHedgeTexture = new Texture(convertPixels, StoneHenge_width, StoneHenge_height, numOFPixels);
	Textures.push_back(stoneHedgeTexture);
	GenerateMipMapForTexture(*Textures[0]);

	SV_MaxMipMapLevel = Textures.size() - 1;

	SV_Texture = *Textures[0];

	SV_DirectionLightDirection = Vector3D(-0.577f, -0.577f, 0.577f);
	SV_DirectionalLightColor = 0xFFC0C0F0;

	SV_PointLightPosition = Vector3D(-1.0f, 0.5f, 1.0f);
	SV_PointLightColor = 0xFFFFFF00;
	SV_PointLightRadius = 10.0f;
	float currentPointLightRadius = 0.0f;

	SV_AmbientTerm = 0.3f;

	float totalTimePassed = 0.0f;

	renderFrameMode = RenderFrameMode::Textured;
	bShowTriangleVertexNormals = false;

	while (RS_Update(pixels, TOTAL_PIXELS))
	{
		timePassed += static_cast<float>(mTimer.Delta());
		totalTimePassed += static_cast<float>(mTimer.Delta());

		if (timePassed > FRAME_RATE)
		{
			Rasterization::ClearBuffers(0xff163d49);

			worldCamera.SetViewMatrix();

			SV_WorldMatrix = Matrix4D::Identity();
			SV_ViewMatrix = worldCamera.GetViewMatrix();
			SV_ProjectionMatrix = projectionMatrix;

			worldCameraFowardVector = worldCamera.GetForwardVector();

			PIXEL_SHADER = PS_WhiteColor;
			for (int i = 0; i < STARS_COUNT; i++)
			{
				Rasterization::DrawPoint(starsVertices[i]);
			}

			/* Point Lighting */
			currentPointLightRadius += FRAME_RATE * sin(totalTimePassed) * 10.0f;
			currentPointLightRadius = Math::Clamp(0.0f, 10.0f, currentPointLightRadius);
			SV_PointLightRadius = currentPointLightRadius;

			if (renderFrameMode == RenderFrameMode::Textured || renderFrameMode == RenderFrameMode::Shaded)
			{
				PIXEL_SHADER = PS_Texture;
				Rasterization::DrawTriangleWithIndexBuffer(stoneHedgeVertices, StoneHenge_indicies, 2532);
			}
			else
			{
				PIXEL_SHADER = PS_GreenColor;
				Rasterization::DrawTriangleOutlinesWithIndexBuffer(stoneHedgeVertices, StoneHenge_indicies, 2532);
			}

			// Input
			if (GetAsyncKeyState(0x31) & 0x01) // 1
			{
				SV_TextureFilter = TextureFilter::NEAREST;
			}
			else if (GetAsyncKeyState(0x32) & 0x01) // 2
			{
				SV_TextureFilter = TextureFilter::BILINEAR;
			}
			else if (GetAsyncKeyState(0x33) & 0x01) // 3
			{
				SV_TextureFilter = TextureFilter::TRILINEAR;
			}

			if (GetAsyncKeyState(0x57) & 0x01) // w
			{
				worldCamera.Translate(0.0f, 0.0f, worldCameraTranslateSpeed * FRAME_RATE);
			}
			else if (GetAsyncKeyState(0x53) & 0x01) // s
			{
				worldCamera.Translate(0.0f, 0.0f, -worldCameraTranslateSpeed * FRAME_RATE);
			}
			else if (GetAsyncKeyState(0x41) & 0x01) // a
			{
				worldCamera.Translate(-worldCameraTranslateSpeed * FRAME_RATE, 0.0f, 0.0f);
			}
			else if (GetAsyncKeyState(0x44) & 0x01) // d
			{
				worldCamera.Translate(worldCameraTranslateSpeed * FRAME_RATE, 0.0f, 0.0f);
			}

			if (GetAsyncKeyState(0x26) & 0x01) // arrow up  
			{
				worldCamera.Rotate(0.0f, worldCameraTranslateSpeed * FRAME_RATE, 0.0f);
			}
			if (GetAsyncKeyState(0x28) & 0x01) // arrow down
			{
				worldCamera.Rotate(0.0f, -worldCameraTranslateSpeed * FRAME_RATE, 0.0f);
			}

			if (GetAsyncKeyState(0x25) & 0x01) // arrow left
			{
				worldCamera.Rotate(-worldCameraTranslateSpeed * FRAME_RATE, 0.0f, 0.0f);
			}
			if (GetAsyncKeyState(0x27) & 0x01) // arrow right
			{
				worldCamera.Rotate(worldCameraTranslateSpeed * FRAME_RATE, 0.0f, 0.0f);
			}


			if (GetAsyncKeyState(0x09) & 0x01) // Tab
			{
				renderFrameMode = (RenderFrameMode)((((int)renderFrameMode) + 1) % 3);
			}

			if (GetAsyncKeyState(0x4E) & 0x01) // N
			{
				bShowTriangleVertexNormals = !bShowTriangleVertexNormals;
			}

			timePassed = 0.0f;
		}

		mTimer.Signal();
	}

	for (unsigned int i = 0; i < Textures.size(); i++)
	{
		delete[] Textures[i]->Pixels;
		delete[] Textures[i];
	}
}

Application::~Application()
{
	RS_Shutdown();

	delete[] pixels;
	delete[] depthBuffer;
	delete[] starsVertices;
	delete[] stoneHedgeVertices;
}
