#pragma once
#include "Graphics/Shaders.h"
#include <iostream>

enum class RenderFrameMode
{
	WireFrame,
	Textured,
	Shaded /* Lighting calculated color gets drawn */
};

unsigned int* pixels = new unsigned int[TOTAL_PIXELS];
float* depthBuffer = new float[TOTAL_PIXELS];

Vertex* starsVertices = new Vertex[STARS_COUNT];

Vertex* stoneHedgeVertices = new Vertex[1457];

Vector3D worldCameraFowardVector;

RenderFrameMode renderFrameMode;

bool bShowTriangleVertexNormals;

struct Rasterization
{
	/* Pixel Drawing*/
private:
	static void DrawPixel(unsigned int x, unsigned int y, float zDepthValue, unsigned int color)
	{
		unsigned int position = Math::Convert2DTo1D(x, y, RASTER_WIDTH);
		if (position >= TOTAL_PIXELS) return;
		if (zDepthValue > depthBuffer[position]) return;

		if ((color & ALPHA_CHANNEL) < ALPHA_CHANNEL)
		{
			color = Math::BlendColor(pixels[position], color);
		}

		depthBuffer[position] = zDepthValue;
		pixels[position] = color;
	}

public:
	static void DrawPoint(Vertex& point)
	{
		Vertex pointCopy = point;

		if (VERTEX_SHADER)
		{
			VERTEX_SHADER(pointCopy);
		}

		pointCopy.PerspectiveDivide();

		IntPoint2D pointPos = Math::ConvertCartesianToScreen(pointCopy);

		unsigned int colorCopy = point.Color;
		if (PIXEL_SHADER)
		{
			PIXEL_SHADER(colorCopy);
		}

		DrawPixel(pointPos.X, pointPos.Y, pointCopy.Z, colorCopy);
	}

	/* Line Drawing */
private:
	static void DrawLineParametricX(Pixel3D& start, Pixel3D& end)
	{
		float ratio = 0.f;

		unsigned int currentX = start.X;
		float currentY = 0.f;

		int xIncrement = ((int)end.X - (int)start.X) >= 0 ? 1 : -1;
		float yIncrement = ((int)end.Y - (int)start.Y) >= 0 ? 0.5f : -0.5f;

		float deltaXRatioRecip = (1.0f / (end.X - start.X));

		while (currentX != end.X)
		{
			unsigned int colorCopy = Math::LerpColor(start.Color, end.Color, ratio);
			if (PIXEL_SHADER)
			{
				PIXEL_SHADER(colorCopy);
			}

			ratio = (static_cast<float>(currentX) - start.X) * deltaXRatioRecip;
			currentY = Math::Lerp(start.Y, end.Y, ratio);

			DrawPixel(currentX, std::floor(currentY + yIncrement), Math::Lerp(start.Z, end.Z, ratio), colorCopy);

			currentX += xIncrement;
		}
	}

	static void DrawLineParametricY(Pixel3D& start, Pixel3D& end)
	{
		float ratio = 0.f;

		unsigned int currentY = start.Y;
		float currentX = 0.f;

		float xIncrement = ((int)end.X - (int)start.X) >= 0 ? 0.5f : -0.5f;
		int yIncrement = ((int)end.Y - (int)start.Y) >= 0 ? 1 : -1;

		float deltaYRatioRecip = (1.0f / (end.Y - start.Y));

		if (yIncrement < 0)
		{
			float tempZ = start.Z;
			start.Z = end.Z;
			end.Z = tempZ;
		}

		while (currentY != end.Y)
		{
			unsigned int colorCopy = Math::LerpColor(start.Color, end.Color, ratio);
			if (PIXEL_SHADER)
			{
				PIXEL_SHADER(colorCopy);
			}

			ratio = (static_cast<float>(currentY) - start.Y) * deltaYRatioRecip;
			currentX = Math::Lerp(start.X, end.X, ratio);

			DrawPixel(static_cast<unsigned int>(std::floor(currentX + xIncrement)), currentY, Math::Lerp(start.Z, end.Z, ratio), colorCopy);

			currentY += yIncrement;
		}
	}

	static void DrawLineParametric(Pixel3D& start, Pixel3D& end)
	{
		unsigned int deltaX = std::abs(static_cast<int>(end.X - start.X));
		unsigned int deltaY = std::abs(static_cast<int>(end.Y - start.Y));

		if (deltaX >= deltaY)
		{
			DrawLineParametricX(start, end);
		}
		else
		{
			DrawLineParametricY(start, end);
		}
	}

	static void DrawLineInProjectionSpace(const Vertex& start, const Vertex& end)
	{
		Vertex startCopy = start;
		Vertex endCopy = end;

		// Clip Line In Projection Space -> discards the line if its behind the near plane
		if (Math::ClipLineInProjectionSpace(startCopy, endCopy)) { return; }

		// Transform vertices into ndc space
		startCopy.PerspectiveDivide();
		endCopy.PerspectiveDivide();

		// Clip line if not in NDC space or discard if not in it at all
		if (!Math::ValidateLineInNDCSpace(startCopy, endCopy)) { return; }

		// Convert 2D cartesian coordinates to screen coordinates
		Pixel3D startPos = Pixel3D(Math::ConvertCartesianToScreen(startCopy), startCopy.Z, startCopy.Color);
		Pixel3D endPos = Pixel3D(Math::ConvertCartesianToScreen(endCopy), endCopy.Z, endCopy.Color);

		// Draw line
		DrawLineParametric(startPos, endPos);
	}

	static void DrawLineInNDCSpace(const Vertex& start, const Vertex& end)
	{
		Vertex startCopy = start;
		Vertex endCopy = end;

		// Clip line if not in NDC space or discard if not in it at all
		if (!Math::ValidateLineInNDCSpace(startCopy, endCopy)) { return; }

		// Convert 2D cartesian coordinates to screen coordinates
		Pixel3D startPos = Pixel3D(Math::ConvertCartesianToScreen(startCopy), startCopy.Z, startCopy.Color);
		Pixel3D endPos = Pixel3D(Math::ConvertCartesianToScreen(endCopy), endCopy.Z, endCopy.Color);

		// Draw line
		DrawLineParametric(startPos, endPos);
	}

	/* Line Drawing */
public:
	/* Draws line in local space, argument vertices are in local space */
	static void DrawLine(const Vertex& start, const Vertex& end)
	{
		// Copy input data and send through shaders
		Vertex startCopy = start;
		Vertex endCopy = end;

		// Use vertex shader to modify copies only
		if (VERTEX_SHADER)
		{
			VERTEX_SHADER(startCopy);
			VERTEX_SHADER(endCopy);
		}

		DrawLineInProjectionSpace(startCopy, endCopy);

		/*// Clip Line In Projection Space -> discards the line if its behind the near plane
		if (Math::ClipLineInProjectionSpace(startCopy, endCopy)) { return; }

		// Transform vertices into ndc space
		startCopy.PerspectiveDivide();
		endCopy.PerspectiveDivide();

		// Clip line if not in NDC space or discard if not in it at all
		if (!Math::ValidateLineInNDCSpace(startCopy, endCopy)) { return; }

		// Convert 2D cartesian coordinates to screen coordinates
		Pixel3D startPos = Pixel3D(Math::ConvertCartesianToScreen(startCopy), startCopy.Z, startCopy.Color);
		Pixel3D endPos = Pixel3D(Math::ConvertCartesianToScreen(endCopy), endCopy.Z, endCopy.Color);

		// Draw line
		DrawLineParametric(startPos, endPos);*/
	}

private:
	static void ReorderTexelsByAscendingY(Texel& a, Texel& b, Texel& c)
	{
		/* Reorder Vertices in Y Ascensing */
		int minY = Math::Min(a.Y, Math::Min(b.Y, c.Y));

		if (minY == b.Y) // b top
		{
			Texel aCopy(a.X, a.Y, a.Z, a.W, a.TexCoordU, a.TexCoordV, a.Color);
			a = Texel(b.X, b.Y, b.Z, b.W, b.TexCoordU, b.TexCoordV, b.Color);
			b = Texel(aCopy.X, aCopy.Y, aCopy.Z, aCopy.W, aCopy.TexCoordU, aCopy.TexCoordV, aCopy.Color);
		}
		else if (minY == c.Y) // c top
		{
			Texel aCopy(a.X, a.Y, a.Z, a.W, a.TexCoordU, a.TexCoordV, a.Color);
			a = Texel(c.X, c.Y, c.Z, c.W, c.TexCoordU, c.TexCoordV, c.Color);
			c = Texel(aCopy.X, aCopy.Y, aCopy.Z, aCopy.W, aCopy.TexCoordU, aCopy.TexCoordV, aCopy.Color);
		}

		int maxY = Math::Max(b.Y, c.Y);

		if (maxY == b.Y) // b bottom
		{
			Texel cCopy(c.X, c.Y, c.Z, c.W, c.TexCoordU, c.TexCoordV, c.Color);
			c = Texel(b.X, b.Y, b.Z, b.W, b.TexCoordU, b.TexCoordV, b.Color);
			b = Texel(cCopy.X, cCopy.Y, cCopy.Z, cCopy.W, cCopy.TexCoordU, cCopy.TexCoordV, cCopy.Color);
		}
	}

	/* Triangle Filling */
private:
	/* All Triangle vertices should be in a clockwise ordering */
	static void DrawFillTriangleBetterBrute(Texel& a, Texel& b, Texel& c)
	{
		int startX = Math::Min(Math::Min(a.X, b.X), c.X);
		int startY = Math::Min(Math::Min(a.Y, b.Y), c.Y);

		int endX = Math::Max(Math::Max(a.X, b.X), c.X);
		int endY = Math::Max(Math::Max(a.Y, b.Y), c.Y);

		Vector3D barycentricDistances = Math::CalculateBarycentricDistances(a, b, c);
		Vector3D alphaBetaGamma;
		float zValue = 1.0f;

		Vector3D recipLinearZ((1.0f / a.W), (1.0f / b.W), (1.0f / c.W));

		for (; startY < endY; startY++)
		{
			for (int x = startX; x < endX; x++)
			{
				if (Math::CalculateAndCheckBarycentricCoordsAndDistances(alphaBetaGamma, a, b, c, IntPoint2D(x, startY), barycentricDistances))
				{
					if (Math::IsBarycentricCoordsInTriangle(alphaBetaGamma))
					{
						unsigned int color = RED;
						if (PIXEL_SHADER != PS_Texture)
						{
							color = Math::BlendColorsWithBarycentricCoordinates(a.Color, b.Color, c.Color, alphaBetaGamma);
						}
						else
						{
							float interpolatedRecipLinearZ = (1.0f / Math::Berp(recipLinearZ.X, recipLinearZ.Y, recipLinearZ.Z, alphaBetaGamma));

							SV_TexCoordU = Math::Berp(a.TexCoordU * recipLinearZ.X, b.TexCoordU * recipLinearZ.Y, c.TexCoordU * recipLinearZ.Z, alphaBetaGamma);
							SV_TexCoordV = Math::Berp(a.TexCoordV * recipLinearZ.X, b.TexCoordV * recipLinearZ.Y, c.TexCoordV * recipLinearZ.Z, alphaBetaGamma);

							SV_TexCoordU *= interpolatedRecipLinearZ;
							SV_TexCoordV *= interpolatedRecipLinearZ;
						}

						// Mipmap
						float wDepthValue = Math::Berp(a.W, b.W, c.W, alphaBetaGamma);
						float r = (wDepthValue - 0.1f) / (9.9f);
						SV_MipMapLevel = (abs(r) * 9.0f);

						if (PIXEL_SHADER)
						{
							PIXEL_SHADER(color);
						}

						zValue = Math::CalculateZDepthValueFromBarycentricCoords(a, b, c, alphaBetaGamma);
						DrawPixel(x, startY, zValue, color);
					}
				}
			}
		}
	}

	static void DrawFillTriangleLariAlgorithm(Texel& a, Texel& b, Texel& c)
	{
		ReorderTexelsByAscendingY(a, b, c);

		/* Algorithm Variables*/
		IntPoint2D currentPixel(a.X, a.Y);

		/* Parametric line algorithm Variables*/
		float ratio = 0.f;
		float deltaYReciprocal_CA = 1.0f / (c.Y - a.Y); // Reciprocal of line ca's delta Y

		/* calculate the line direction*/
		int lineDirection = Math::ImplicitLine(c, a, b) > 0.0f ? 1 : -1;

		/* Calculate the barycentric distances */
		Vector3D bDistances = Math::CalculateBarycentricDistances(a, b, c);
		Vector3D alphaBetaGamma;

		/* Reciprocal linear Z */
		Vector3D linearZReciprocal((1.0f / a.W), (1.0f / b.W), (1.0f / c.W));

		float deltaNearFarPlanesReciprocal = (1.0f / (SV_FarPlane - SV_NearPlane));

		// Clamp Triangle if outside NDC/Screen Space
		currentPixel.Y = Math::Clamp(0, RASTER_HEIGHT, a.Y);

		while (currentPixel.Y != c.Y)
		{
			if (currentPixel.Y > RASTER_HEIGHT) { return; }

			// Clamp X so it stays inside screen space
			if (lineDirection > 0)
			{
				if (currentPixel.X > (RASTER_WIDTH - 1))
				{
					currentPixel.Y++;
					ratio = static_cast<float>(currentPixel.Y - a.Y) * deltaYReciprocal_CA;
					currentPixel.X = static_cast<unsigned int>(Math::Lerp(a.X, c.X, ratio));

					continue;
				}
				else if (currentPixel.X < 0.0f)
				{
					currentPixel.X = 0.0;
				}
			}
			else if (currentPixel.X >= RASTER_WIDTH)
			{
				currentPixel.X = RASTER_WIDTH - 1;
			}
			else if (currentPixel.X < 0.0f)
			{
				currentPixel.Y++;
				ratio = static_cast<float>(currentPixel.Y - a.Y) * deltaYReciprocal_CA;
				currentPixel.X = Math::Clamp(0, RASTER_WIDTH - 1, static_cast<int>(Math::Lerp(a.X, c.X, ratio)));

				continue;
			}

			alphaBetaGamma = Math::CalculateBarycentricCoords(a, b, c, currentPixel);
			alphaBetaGamma /= bDistances;

			if (alphaBetaGamma.Y < 0.0f) // out side line ab 
			{
				currentPixel.X += lineDirection;
			}
			else if ((alphaBetaGamma.X >= 0.0f && alphaBetaGamma.X <= 1.0f)
				&& (alphaBetaGamma.Y >= 0.0f && alphaBetaGamma.Y <= 1.0f)
				&& (alphaBetaGamma.Z >= 0.0f && alphaBetaGamma.Z <= 1.0f)) // draw pixel since point is in the triangle
			{
				if (renderFrameMode == RenderFrameMode::Shaded)
				{
					float zDepthValue = Math::CalculateZDepthValueFromBarycentricCoords(a, b, c, alphaBetaGamma);
					unsigned int lightColor = Math::BlendColorsWithBarycentricCoordinates(a.Color, b.Color, c.Color, alphaBetaGamma);

					DrawPixel(currentPixel.X, currentPixel.Y, zDepthValue, lightColor);
					currentPixel.X += lineDirection;

					continue;
				}

				unsigned int color = RED;
				if (PIXEL_SHADER != PS_Texture)
				{
					color = Math::BlendColorsWithBarycentricCoordinates(a.Color, b.Color, c.Color, alphaBetaGamma);
				}
				else
				{
					float interpolatedRecipLinearZ = (1.0f / Math::Berp(linearZReciprocal.X, linearZReciprocal.Y, linearZReciprocal.Z, alphaBetaGamma));

					SV_TexCoordU = Math::Berp(a.TexCoordU * linearZReciprocal.X, b.TexCoordU * linearZReciprocal.Y, c.TexCoordU * linearZReciprocal.Z, alphaBetaGamma);
					SV_TexCoordV = Math::Berp(a.TexCoordV * linearZReciprocal.X, b.TexCoordV * linearZReciprocal.Y, c.TexCoordV * linearZReciprocal.Z, alphaBetaGamma);

					SV_TexCoordU *= interpolatedRecipLinearZ;
					SV_TexCoordV *= interpolatedRecipLinearZ;

					// Mipmap
					float wDepthValue = Math::Berp(a.W, b.W, c.W, alphaBetaGamma);
					float r = (wDepthValue - 0.1f) * deltaNearFarPlanesReciprocal;
					SV_MipMapLevel = static_cast<unsigned int>(r * static_cast<float>(SV_MaxMipMapLevel));
				}

				/* Pixel depth value for depth buffer */
				float zDepthValue = Math::CalculateZDepthValueFromBarycentricCoords(a, b, c, alphaBetaGamma);

				if (PIXEL_SHADER)
				{
					PIXEL_SHADER(color);
				}

				unsigned int lightColor = Math::BlendColorsWithBarycentricCoordinates(a.Color, b.Color, c.Color, alphaBetaGamma);
				color = Math::ModulateColors(color, lightColor);
				//color = Math::ModulateColors(color, directionLightColor);

				DrawPixel(currentPixel.X, currentPixel.Y, zDepthValue, color);
				currentPixel.X += lineDirection;
			}
			else
			{
				currentPixel.Y++;
				ratio = static_cast<float>(currentPixel.Y - a.Y) * deltaYReciprocal_CA;
				currentPixel.X = static_cast<unsigned int>(Math::Lerp(a.X, c.X, ratio));
			}
		}
	}

	/* Triangle Drawing */
private:
	static void DrawTriangleInProjectionSpace(const Vertex& a, const Vertex& b, const Vertex& c)
	{
		Vertex aCopy = a;
		Vertex bCopy = b;
		Vertex cCopy = c;

		aCopy.PerspectiveDivide();
		bCopy.PerspectiveDivide();
		cCopy.PerspectiveDivide();

		// Back Face Culling
		Vector3D vecA(aCopy.X, aCopy.Y, aCopy.Z);
		Vector3D vecB(bCopy.X, bCopy.Y, bCopy.Z);
		Vector3D vecC(cCopy.X, cCopy.Y, cCopy.Z);

		Vector3D perpVec = Vector3D::CrossProduct((vecA - vecB), (vecA - vecC));
		perpVec.Normalize();
		if (Vector3D::DotProduct(perpVec, worldCameraFowardVector) >= 0) { return; }

		// Convert 2D cartesian coordinates to screen coordinates
		Texel aPos = Texel(Math::ConvertCartesianToScreen(aCopy), aCopy.Z, aCopy.W, a.TexCoordU, a.TexCoordV, a.Color);
		Texel bPos = Texel(Math::ConvertCartesianToScreen(bCopy), bCopy.Z, bCopy.W, b.TexCoordU, b.TexCoordV, b.Color);
		Texel cPos = Texel(Math::ConvertCartesianToScreen(cCopy), cCopy.Z, cCopy.W, c.TexCoordU, c.TexCoordV, c.Color);

		// Fill Triangle
		DrawFillTriangleLariAlgorithm(aPos, bPos, cPos);

		// Outline the triangles being drawn
#if SHOW_TRIANGLE_OUTLINES
		auto pixelShaderTemp = PIXEL_SHADER;
		PIXEL_SHADER = PS_GreenColor;

		/* Move lines Z position 25% closer so depth buffer will allow them to be drawn infront of triangle*/
		aCopy.Z -= aCopy.Z * 0.25f;
		bCopy.Z -= bCopy.Z * 0.25f;
		cCopy.Z -= cCopy.Z * 0.25f;

		// Draw Triangle outline
		DrawLineInNDCSpace(aCopy, bCopy);
		DrawLineInNDCSpace(bCopy, cCopy);
		DrawLineInNDCSpace(aCopy, cCopy);

		PIXEL_SHADER = pixelShaderTemp;
#endif // SHOW_TRIANGLE_OUTLINES

		if (bShowTriangleVertexNormals)
		{
			auto pixelShaderTemp = PIXEL_SHADER;
			PIXEL_SHADER = PS_RedColor;

			float normalLengthScale = 0.15f;
			aCopy = a + (a.Normal * normalLengthScale);
			bCopy = b + (b.Normal * normalLengthScale);
			cCopy = c + (c.Normal * normalLengthScale);

			DrawLineInProjectionSpace(a, aCopy);
			DrawLineInProjectionSpace(b, bCopy);
			DrawLineInProjectionSpace(c, cCopy);

			PIXEL_SHADER = pixelShaderTemp;
		}
	}

	static void DrawTriangleOutlineInProjectionSpace(const Vertex& a, const Vertex& b, const Vertex& c)
	{
		Vertex aCopy = a;
		Vertex bCopy = b;
		Vertex cCopy = c;

		aCopy.PerspectiveDivide();
		bCopy.PerspectiveDivide();
		cCopy.PerspectiveDivide();

		// Back Face Culling
		Vector3D vecA(aCopy.X, aCopy.Y, aCopy.Z);
		Vector3D vecB(bCopy.X, bCopy.Y, bCopy.Z);
		Vector3D vecC(cCopy.X, cCopy.Y, cCopy.Z);

		Vector3D perpVec = Vector3D::CrossProduct((vecA - vecB), (vecA - vecC));
		perpVec.Normalize();
		if (Vector3D::DotProduct(perpVec, worldCameraFowardVector) >= 0) { return; }

		// Draw Triangle
		DrawLineInNDCSpace(aCopy, bCopy);
		DrawLineInNDCSpace(bCopy, cCopy);
		DrawLineInNDCSpace(aCopy, cCopy);

		/* Shows the vertex normals of the triangles vertices */
		if (bShowTriangleVertexNormals)
		{
			auto pixelShaderTemp = PIXEL_SHADER;
			PIXEL_SHADER = PS_RedColor;

			float normalLengthScale = 0.15f;
			aCopy = a + (a.Normal * normalLengthScale);
			bCopy = b + (b.Normal * normalLengthScale);
			cCopy = c + (c.Normal * normalLengthScale);

			DrawLineInProjectionSpace(a, aCopy);
			DrawLineInProjectionSpace(b, bCopy);
			DrawLineInProjectionSpace(c, cCopy);

			PIXEL_SHADER = pixelShaderTemp;
		}
	}

	/* Triangle Drawing */
public:
	static void DrawTriangleOutline(const Vertex& a, const Vertex& b, const Vertex& c)
	{
		// Copy input data and send through shaders
		Vertex aCopy = a;
		Vertex bCopy = b;
		Vertex cCopy = c;

		// Use vertex shader to modify copies only
		if (VERTEX_SHADER)
		{
			VERTEX_SHADER(aCopy);
			VERTEX_SHADER(bCopy);
			VERTEX_SHADER(cCopy);
		}

		// Triangle clipping in projection space
		bool vertAClipCheck = aCopy.Z < 0.0f;
		bool vertBClipCheck = bCopy.Z < 0.0f;
		bool vertCClipCheck = cCopy.Z < 0.0f;

		if (vertAClipCheck)
		{
			if (vertBClipCheck) // edge ab is behind near plane
			{
				if (vertCClipCheck) { return; }

				Math::ClipLineZByParametricRatio(cCopy, aCopy);
				Math::ClipLineZByParametricRatio(cCopy, bCopy);
			}
			else if (vertCClipCheck)// edge ca is behind near plane
			{
				Math::ClipLineZByParametricRatio(bCopy, cCopy);
				Math::ClipLineZByParametricRatio(bCopy, aCopy);
			}
			else
			{
				Vertex splitA = aCopy;

				Math::ClipLineZByParametricRatio(bCopy, aCopy);
				Math::ClipLineZByParametricRatio(cCopy, splitA);

				//DrawTriangleInProjectionSpace(aCopy, bCopy, cCopy);
				DrawTriangleOutlineInProjectionSpace(aCopy, cCopy, splitA);
			}
		}
		else if (vertBClipCheck) // vert B is behind near plane, split both lines cb, ab
		{
			if (vertCClipCheck)// edge bc is behind near plane
			{
				Math::ClipLineZByParametricRatio(aCopy, bCopy);
				Math::ClipLineZByParametricRatio(aCopy, cCopy);
			}
			else
			{
				Vertex splitB = bCopy;

				// Split cb, ab
				Math::ClipLineZByParametricRatio(cCopy, bCopy);
				Math::ClipLineZByParametricRatio(aCopy, splitB);

				//DrawTriangleInProjectionSpace(bCopy, cCopy, aCopy);
				DrawTriangleOutlineInProjectionSpace(bCopy, aCopy, splitB);
			}
		}
		else if (vertCClipCheck) // vert C is behind near plane, split both lines ac, bc
		{
			Vertex splitC = cCopy;

			// Split cb, ab
			Math::ClipLineZByParametricRatio(aCopy, cCopy);
			Math::ClipLineZByParametricRatio(bCopy, splitC);

			//DrawTriangleInProjectionSpace(cCopy, aCopy, bCopy);
			DrawTriangleOutlineInProjectionSpace(cCopy, bCopy, splitC);
		}

		//PIXEL_SHADER = PS_GreenColor;
		DrawTriangleOutlineInProjectionSpace(aCopy, bCopy, cCopy);
	}

	static void DrawTriangle(const Vertex& a, const Vertex& b, const Vertex& c)
	{
		// Copy input data and send through shaders
		Vertex aCopy = a;
		Vertex bCopy = b;
		Vertex cCopy = c;

		// Use vertex shader to modify copies only
		if (VERTEX_SHADER)
		{
			VERTEX_SHADER(aCopy);
			VERTEX_SHADER(bCopy);
			VERTEX_SHADER(cCopy);
		}

		// Triangle clipping in projection space
		bool vertAClipCheck = aCopy.Z < 0.0f;
		bool vertBClipCheck = bCopy.Z < 0.0f;
		bool vertCClipCheck = cCopy.Z < 0.0f;

		if (vertAClipCheck)
		{
			if (vertBClipCheck) // edge ab is behind near plane
			{
				if (vertCClipCheck) { return; }

				Math::ClipLineZByParametricRatio(cCopy, aCopy);
				Math::ClipLineZByParametricRatio(cCopy, bCopy);
			}
			else if (vertCClipCheck)// edge ca is behind near plane
			{
				Math::ClipLineZByParametricRatio(bCopy, cCopy);
				Math::ClipLineZByParametricRatio(bCopy, aCopy);
			}
			else
			{
				Vertex splitA = aCopy;

				Math::ClipLineZByParametricRatio(bCopy, aCopy);
				Math::ClipLineZByParametricRatio(cCopy, splitA);

				//DrawTriangleInProjectionSpace(aCopy, bCopy, cCopy);
				DrawTriangleInProjectionSpace(aCopy, cCopy, splitA);
			}
		}
		else if (vertBClipCheck) // vert B is behind near plane, split both lines cb, ab
		{
			if (vertCClipCheck)// edge bc is behind near plane
			{
				Math::ClipLineZByParametricRatio(aCopy, bCopy);
				Math::ClipLineZByParametricRatio(aCopy, cCopy);
			}
			else
			{
				Vertex splitB = bCopy;

				// Split cb, ab
				Math::ClipLineZByParametricRatio(cCopy, bCopy);
				Math::ClipLineZByParametricRatio(aCopy, splitB);

				//DrawTriangleInProjectionSpace(bCopy, cCopy, aCopy);
				DrawTriangleInProjectionSpace(bCopy, aCopy, splitB);
			}
		}
		else if (vertCClipCheck) // vert C is behind near plane, split both lines ac, bc
		{
			Vertex splitC = cCopy;

			// Split cb, ab
			Math::ClipLineZByParametricRatio(aCopy, cCopy);
			Math::ClipLineZByParametricRatio(bCopy, splitC);

			//DrawTriangleInProjectionSpace(cCopy, aCopy, bCopy);
			DrawTriangleInProjectionSpace(cCopy, bCopy, splitC);
		}

		//PIXEL_SHADER = PS_GreenColor;
		DrawTriangleInProjectionSpace(aCopy, bCopy, cCopy);
	}

	static void DrawTriangleWithIndexBuffer(const Vertex* vertices, const unsigned int* indexBuffer, const unsigned int indicesCount)
	{
		for (unsigned int i = 0; i < indicesCount; i += 3)
		{
			DrawTriangle(vertices[indexBuffer[i]], vertices[indexBuffer[i + 1]], vertices[indexBuffer[i + 2]]);
		}
	}

	static void DrawTriangleOutlinesWithIndexBuffer(const Vertex* vertices, const unsigned int* indexBuffer, const unsigned int indicesCount)
	{
		for (unsigned int i = 0; i < indicesCount; i += 3)
		{
			DrawTriangleOutline(vertices[indexBuffer[i]], vertices[indexBuffer[i + 1]], vertices[indexBuffer[i + 2]]);
		}
		/*for (unsigned int i = 2; i < indicesCount; i += 3)
		{
			DrawTriangleOutline(vertices[indexBuffer[i - 2]], vertices[indexBuffer[i - 1]], vertices[indexBuffer[i]]);
		}*/
	}

	/* Color / Depth Buffer stuff */
public:
	static void ClearBuffers(unsigned int color)
	{
		for (int i = 0; i < TOTAL_PIXELS; i++)
		{
			pixels[i] = color;
			depthBuffer[i] = 1.0f;
		}
	}
};
