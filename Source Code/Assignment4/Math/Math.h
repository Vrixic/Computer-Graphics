#pragma once
#include "Graphics/Texel.h"
#include "Matrix4D.h"
#include "Graphics/Vertex.h"
#include "Graphics/Texture.h"

struct Math
{
public:
	template<class T>
	/* Return minimum between a and b */
	inline static T Min(T a, T b)
	{
		return a < b ? a : b;
	}

	template<class T>
	/* Return maximum between a and b */
	inline static T Max(T a, T b)
	{
		return a > b ? a : b;
	}

	template<class T>
	inline static T Clamp(T min, T max, T test)
	{
		T r = test;
		if (test > max)
		{
			r = max;
		}
		else if (test < min)
		{
			r = min;
		}

		return r;
	}

	/*inline float RandomRange(float min, float max)
	{
		float r = rand() / (float)RAND_MAX;
		return (max - min) * r + min;
	}*/

	static unsigned int Convert2DTo1D(int x, int y, int textureWidth)
	{
		return y * textureWidth + x;
	}

	static float Lerp(float start, float end, float ratio)
	{
		return static_cast<float>(end - start) * ratio + start;
	}

	inline static void LerpVerticesByRatio(Vertex& start, Vertex& end, float ratio)
	{
		start.X = Math::Lerp(start.X, end.X, ratio);
		start.Y = Math::Lerp(start.Y, end.Y, ratio);
		start.Z = Math::Lerp(start.Z, end.Z, ratio);
		start.W = Math::Lerp(start.W, end.W, ratio);

		start.Color = Math::LerpColor(start.Color, end.Color, ratio);

		start.TexCoordU = Math::Lerp(start.TexCoordU, end.TexCoordU, ratio);
		start.TexCoordV = Math::Lerp(start.TexCoordV, end.TexCoordV, ratio);
	}

	/* Blends two pixels/colors based on the alpha of the newColor -- Transparency Blend */
	static unsigned int BlendColor(unsigned int lastColor, unsigned int newColor)
	{
		float alpha = static_cast<float>((newColor & ALPHA_CHANNEL) >> 24) / 255;

		unsigned int redChannel = static_cast<unsigned int>(Lerp(static_cast<float>((lastColor & RED_CHANNEL) >> 16), static_cast<float>((newColor & RED_CHANNEL) >> 16), alpha)) << 16;
		unsigned int blueChannel = static_cast<unsigned int>(Lerp(static_cast<float>(lastColor & BLUE_CHANNEL), static_cast<float>(newColor & BLUE_CHANNEL), alpha));
		unsigned int greenChannel = static_cast<unsigned int>(Lerp(static_cast<float>((lastColor & GREEN_CHANNEL) >> 8), static_cast<float>((newColor & GREEN_CHANNEL) >> 8), alpha)) << 8;
		unsigned int alphaChannel = static_cast<unsigned int>(Lerp(static_cast<float>((lastColor & ALPHA_CHANNEL) >> 24), static_cast<float>((newColor & ALPHA_CHANNEL) >> 24), alpha)) << 24;

		return (redChannel | blueChannel | greenChannel | alphaChannel);
	}

	static unsigned int AddColor(unsigned int color1, unsigned int color2)
	{
		unsigned int redChannel = Clamp(0u, 255u, (((color1 & RED_CHANNEL) >> 16) + ((color2 & RED_CHANNEL) >> 16)) << 16);
		unsigned int blueChannel = Clamp(0u, 255u, ((color1 & BLUE_CHANNEL) + (color2 & BLUE_CHANNEL)));
		unsigned int greenChannel = Clamp(0u, 255u, (((color1 & GREEN_CHANNEL) >> 8) + ((color2 & GREEN_CHANNEL) >> 8)) << 8);
		unsigned int alphaChannel = Clamp(0u, 255u, (((color1 & ALPHA_CHANNEL) >> 24) +  ((color2 & ALPHA_CHANNEL) >> 24)) << 24);
	
		return (redChannel | blueChannel | greenChannel | alphaChannel);
	}

	static unsigned int CombineColors(unsigned int color1, unsigned int color2)
	{
		return AddColor(color1, color2);
	}

	static unsigned int ModulateColors(unsigned int color1, unsigned int color2)
	{
		float r = 1.0f / 255.0f;

		unsigned int redChannel = static_cast<unsigned int>(Clamp(0.0f, 1.0f, (((color1 & RED_CHANNEL) >> 16) * r) * (((color2 & RED_CHANNEL) >> 16) * r)) * 255.0f) << 16;
		unsigned int blueChannel = static_cast<unsigned int>(Clamp(0.0f, 1.0f, ((color1 & BLUE_CHANNEL) * r) * ((color2 & BLUE_CHANNEL) * r)) * 255.0f);
		unsigned int greenChannel = static_cast<unsigned int>(Clamp(0.0f, 1.0f, (((color1 & GREEN_CHANNEL) >> 8) * r) * (((color2 & GREEN_CHANNEL) >> 8) * r)) * 255.0f) << 8;
		unsigned int alphaChannel = static_cast<unsigned int>(Clamp(0.0f, 1.0f, (((color1 & ALPHA_CHANNEL) >> 24) * r) * (((color2 & ALPHA_CHANNEL) >> 24) * r)) * 255.0f) << 24;

		return (redChannel | blueChannel | greenChannel | alphaChannel);
	}

	/* Lerp two pixels/color based on a ratio -- Mixing Colors */
	static unsigned int LerpColor(unsigned int startColor, unsigned int endColor, float ratio)
	{
		unsigned int redChannel = (static_cast<unsigned int>(Lerp(static_cast<float>((startColor & RED_CHANNEL) >> 16), static_cast<float>((endColor & RED_CHANNEL) >> 16), ratio))) << 16;
		unsigned int greenChannel = (static_cast<unsigned int>(Lerp(static_cast<float>((startColor & GREEN_CHANNEL) >> 8), static_cast<float>((endColor & GREEN_CHANNEL) >> 8), ratio))) << 8;
		unsigned int blueChannel = static_cast<unsigned int>(Lerp(static_cast<float>((startColor & BLUE_CHANNEL)), static_cast<float>((endColor & BLUE_CHANNEL)), ratio));

		return (redChannel | greenChannel | blueChannel | ALPHA_CHANNEL);
	}

	static unsigned int BerpColor(unsigned int color1, unsigned int color2, unsigned int color3, Vector3D& alphaBetaGamma)
	{
		unsigned int redChannel = Math::Clamp(0u, 255u, (static_cast<unsigned int>(Berp(static_cast<float>((color1 & RED_CHANNEL) >> 16), static_cast<float>((color2 & RED_CHANNEL) >> 16), static_cast<float>((color3 & RED_CHANNEL) >> 16), alphaBetaGamma))) << 16);
		unsigned int greenChannel = Math::Clamp(0u, 255u, (static_cast<unsigned int>(Berp(static_cast<float>((color1 & GREEN_CHANNEL) >> 8), static_cast<float>((color2 & GREEN_CHANNEL) >> 8), static_cast<float>((color3 & GREEN_CHANNEL) >> 8), alphaBetaGamma))) << 8);
		unsigned int blueChannel = Math::Clamp(0u, 255u, static_cast<unsigned int>(Berp(static_cast<float>((color1 & BLUE_CHANNEL)), static_cast<float>((color2 & BLUE_CHANNEL)), static_cast<float>((color3 & BLUE_CHANNEL)), alphaBetaGamma)));

		return (redChannel | greenChannel | blueChannel | ALPHA_CHANNEL);
	}

	static unsigned int BlendColorWithBarycentricCoordinates(unsigned int color, Vector3D& barycentricCoordinates)
	{
		unsigned int redChannel = (static_cast<unsigned int>(((color & RED_CHANNEL) >> 16) * barycentricCoordinates.X)) << 16;
		unsigned int greenChannel = (static_cast<unsigned int>(((color & GREEN_CHANNEL) >> 8) * barycentricCoordinates.Y)) << 8;
		unsigned int blueChannel = (static_cast<unsigned int>((color & BLUE_CHANNEL) * barycentricCoordinates.Z));

		return (redChannel | greenChannel | blueChannel | ALPHA_CHANNEL);
	}

	static unsigned int BlendColorsWithBarycentricCoordinates(unsigned int color1, unsigned int color2, unsigned int color3, Vector3D& barycentricCoordinates)
	{
		unsigned int red = static_cast<unsigned int>((((color1 & RED_CHANNEL) >> 16) * barycentricCoordinates.X) + (((color2 & RED_CHANNEL) >> 16) * barycentricCoordinates.Y) + (((color3 & RED_CHANNEL) >> 16) * barycentricCoordinates.Z)) << 16;
		unsigned int green = static_cast<unsigned int>((((color1 & GREEN_CHANNEL) >> 8) * barycentricCoordinates.X) + (((color2 & GREEN_CHANNEL) >> 8) * barycentricCoordinates.Y) + (((color3 & GREEN_CHANNEL) >> 8) * barycentricCoordinates.Z)) << 8;
		unsigned int blue = static_cast<unsigned int>(((color1 & BLUE_CHANNEL) * barycentricCoordinates.X) + ((color2 & BLUE_CHANNEL) * barycentricCoordinates.Y) + ((color3 & BLUE_CHANNEL) * barycentricCoordinates.Z));

		return (red | green | blue | ALPHA_CHANNEL);
	}

	static void MultiplyVertexByMatrix(Vertex& v, const Matrix4D& m)
	{
		Vector4D p = (m * v);

		v.X = p.X;
		v.Y = p.Y;
		v.Z = p.Z;
		v.W = p.W;
	}

	static Matrix4D OrthogonalInverseMatrix(Matrix4D m)
	{
		// Transpose matrix 3x3
		Matrix4D result
		(
			m(0, 0), m(1, 0), m(2, 0), 0.0f,
			m(0, 1), m(1, 1), m(2, 1), 0.0f,
			m(0, 2), m(1, 2), m(2, 2), 0.0f,
			0.0f, 0.0f, 0.0f, m(3, 3)
		);

		// Take position vector from incoming matrix
		Vector4D positionVector(m(3, 0), m(3, 1), m(3, 2), 1.0f);

		// Multiply transposed 3x3 matrix by the position vector then negate the result
		positionVector = -(result * positionVector);

		// Update the transposed matrix's position vector to the negate result position vector
		result.SetTranslation(Vector3D(positionVector.X, positionVector.Y, positionVector.Z));

		return result;
	}

	/* NDC to Screen*/
	static IntPoint2D ConvertCartesianToScreen(const Vertex& v)
	{
		return IntPoint2D(static_cast<int>(((v.X + 1.0f) * (0.5f * (RASTER_WIDTH - 1)))), static_cast<int>((1.0f - v.Y) * (0.5f * (RASTER_HEIGHT - 1))));
	}

	static unsigned int ConvertBGRAToARGB(unsigned int bgraColor)
	{
		unsigned int redChannel = (bgraColor & 0x0000ff00) << 8;
		unsigned int blueChannel = (bgraColor & 0xff000000) >> 24;
		unsigned int greenChannel = (bgraColor & 0x00ff0000) >> 8;
		unsigned int alphaChannel = (bgraColor & 0x000000ff) << 24;

		return (redChannel | blueChannel | greenChannel | alphaChannel);
	}

	/* Converts Degrees to radians */
	inline static float DegreesToRadians(float degrees)
	{
		return static_cast<float>(((PI / 180.0f) * degrees));
	}

	/* Returns the prespective projection matrix */
	static Matrix4D GetProjectionMatrix(unsigned int windowWidth, unsigned int windowHeight, float verticalFOV, float nearPlane, float farPlane)
	{
		Matrix4D projectionMatrix;
		float aspectRatio = static_cast<float>(windowHeight) / static_cast<float>(windowWidth);
		float yScale = static_cast<float>((cos(Math::DegreesToRadians(verticalFOV * 0.5f)) / sin(Math::DegreesToRadians(verticalFOV * 0.5f))));
		float xScale = yScale * aspectRatio;

		projectionMatrix.SetIdentity();

		projectionMatrix(0, 0) = xScale;
		projectionMatrix(1, 1) = yScale;
		projectionMatrix(2, 2) = (farPlane / (farPlane - nearPlane));
		projectionMatrix(2, 3) = 1.0f;
		projectionMatrix(3, 2) = -(farPlane * nearPlane) / (farPlane - nearPlane);
		projectionMatrix(3, 3) = 0.0f;

		return projectionMatrix;
	}

	/* Checks a point reated to a line
	* @Param 'p1, p2' : makes up the line
	* @Param 'tp' : point being tested to the line
	*
	* returns (0 = point on the line), ( less than 0 = behind the line ), (greater than 0 == in front of the line)
	*	(size is of the number is related to the distance from line to point)
	*/
	static float ImplicitLine(IntPoint2D point1, IntPoint2D point2, IntPoint2D testPoint)
	{
		return static_cast<float>(((point1.Y - point2.Y) * testPoint.X) + ((point2.X - point1.X) * testPoint.Y) + (point1.X * point2.Y) - (point1.Y * point2.X));
	}

	/* Calculates barycentric coordinates
	* @Param 'outBYA': if calculated BYA values are inside the triangle then this vector will be filled in with correct ratios
	* @Param 'a, b, c': indicates the triangles vertices
	* @Param 'testPoint': indicates the point being tested
	* @Param 'barycentricDistances': an array of 3 with alpha, beta, gamma distances to thier respective lines
	*
	* returns a vector containing the barycentric coordinates (X, Y, Z) = (alpha, beta, gamma)
	*/
	static bool CalculateAndCheckBarycentricCoordsAndDistances(Vector3D& outAlphaBetaGamma, IntPoint2D& a, IntPoint2D& b, IntPoint2D& c, IntPoint2D testPoint, Vector3D& barycentricDistances)
	{
		outAlphaBetaGamma.X = ImplicitLine(b, c, testPoint);
		outAlphaBetaGamma.Y = ImplicitLine(c, a, testPoint);
		outAlphaBetaGamma.Z = ImplicitLine(a, b, testPoint);

		if (outAlphaBetaGamma.X > barycentricDistances.X || outAlphaBetaGamma.Y > barycentricDistances.Y || outAlphaBetaGamma.Z > barycentricDistances.Z)
		{
			return false;
		}

		outAlphaBetaGamma /= barycentricDistances;

		return true;
	}

	/* Calculates barycentric coordinates
	* @Param 'a, b, c': indicates the triangles vertices
	* @Param 'testPoint': indicates the point being tested
	*
	* returns a vector containing the barycentric coordinates (X, Y, Z) = (alpha, beta, gamma)
	*/
	static Vector3D CalculateBarycentricCoords(IntPoint2D& a, IntPoint2D& b, IntPoint2D& c, IntPoint2D testPoint)
	{
		Vector3D abg;
		abg.X = ImplicitLine(b, c, testPoint);
		abg.Y = ImplicitLine(c, a, testPoint);
		abg.Z = ImplicitLine(a, b, testPoint);

		return abg;
	}

	/* Calculates the barycentric distances for alpha, beta, and gamma
	* @Param 'a, b, c': triangle vertices
	*/
	static Vector3D CalculateBarycentricDistances(IntPoint2D& a, IntPoint2D& b, IntPoint2D& c)
	{
		Vector3D abgDistances(0.0f);

		abgDistances.X = ImplicitLine(b, c, a);
		abgDistances.Y = ImplicitLine(c, a, b);
		abgDistances.Z = ImplicitLine(a, b, c);

		return abgDistances;
	}

	/* Barycentric Interplation */
	static float Berp(float inA, float inB, float inC, Vector3D& alphaBetaGamma)
	{
		return (inA * alphaBetaGamma.X + inB * alphaBetaGamma.Y + inC * alphaBetaGamma.Z);
	}

	/* Calculates the z depth value by barycentric interplating the z components of a, b, c using barycentric coordinates */
	static float CalculateZDepthValueFromBarycentricCoords(Pixel3D& a, Pixel3D& b, Pixel3D& c, Vector3D& alphaBetaGamma)
	{
		return Berp(a.Z, b.Z, c.Z, alphaBetaGamma);
	}

	/* Checks if the barycentric coordinates are inside a triangle
	* @Param 'bya': an vector corresponding to beta, gamma, alpha
	*/
	inline static bool IsBarycentricCoordsInTriangle(Vector3D& abg)
	{
		return (abg.X >= 0.0f && abg.X <= 1.0f) && (abg.Y >= 0.0f && abg.Y <= 1.0f) && (abg.Z >= 0.0f && abg.Z <= 1.0f);
	}

	inline static void VertexPerspectiveDivide(Vertex& vertex)
	{
		float r = 1.0f / vertex.W;

		vertex.X *= r;
		vertex.Y *= r;
		vertex.Z *= r;
	}

	/* Graphics Stuff*/
	inline static unsigned int CalculateBilinearTextureFilter(float uRatio, float vRatio, unsigned int currentTexel, Texture& texture)
	{
		unsigned int nextPixel = (currentTexel + 1);
		unsigned int bottomPixel = (currentTexel + texture.Width);
		unsigned int diagonalPixel = (bottomPixel + 1);

		unsigned int color1 = Math::LerpColor((texture.Pixels[currentTexel % texture.NumOfPixels]), (texture.Pixels[nextPixel % texture.NumOfPixels]), uRatio);
		unsigned int color2 = Math::LerpColor((texture.Pixels[bottomPixel % texture.NumOfPixels]), (texture.Pixels[diagonalPixel % texture.NumOfPixels]), uRatio);

		return static_cast<unsigned int>(Math::LerpColor(color1, color2, vRatio));
	}

	inline static bool ValidateLineInNDCSpace(Vertex& a, Vertex& b)
	{
		if (a.Y > 1.0f )
		{
			if (b.Y > 1.0f) { return false; }

			float ratio = (a.Y - 1.0f) / (b.Y - a.Y);
			LerpVerticesByRatio(a, b, ratio);
		}
		else if (b.Y > 1.0f) 
		{  
			float ratio = (b.Y - 1.0f) / (a.Y - b.Y);

			LerpVerticesByRatio(b, a, ratio);
		}

		if (a.Y < -1.0f)
		{
			if (b.Y < -1.0f) { return false; }

			float ratio = -(a.Y + 1.0f) / (b.Y - a.Y);
			LerpVerticesByRatio(a, b, ratio);
		}
		else if (b.Y < -1.0f)
		{
			float ratio = -(b.Y + 1.0f) / (a.Y - b.Y);
			LerpVerticesByRatio(b, a, ratio);
		}

		if (a.X > 1.0f)
		{
			if (b.X > 1.0f) { return false; }

			float ratio = -(a.X - 1.0f) / (b.X - a.X);
			LerpVerticesByRatio(a, b, ratio);
		}
		else if (b.X > 1.0f)
		{
			float ratio = -(b.X - 1.0f) / (a.X - b.X);
			LerpVerticesByRatio(b, a, ratio);
		}

		if (a.X < -1.0f)
		{
			if (b.X < -1.0f) { return false; }

			float ratio = -(a.X + 1.0f) / (b.X - a.X);
			LerpVerticesByRatio(a, b, ratio);
		}
		else if (b.X < -1.0f)
		{
			float ratio = -(b.X + 1.0f) / (a.X - b.X);
			LerpVerticesByRatio(b, a, ratio);
		}

		return true;
	}

	inline static void ClipLineZByParametricRatio(Vertex& a, Vertex& b)
	{
		// Perform Clipping
		float ratio = b.Z / (b.Z - a.Z);
		LerpVerticesByRatio(b, a, ratio);
	}

	/* Returns if line is fully outside the near plane */
	inline static bool ClipLineInProjectionSpace(Vertex& a, Vertex& b)
	{
		if (b.Z  < 0.0f)
		{
			if (a.Z == b.Z || (a.Z < 0.0f)) return true;

			ClipLineZByParametricRatio(a, b);
		}
		else if (a.Z < 0.0f)
		{
			if (a.Z == b.Z || (b.Z < 0.0f)) return true;

			ClipLineZByParametricRatio(b, a);
		}

		return false;
	}
};
