#pragma once

struct Texture
{
	unsigned int* Pixels;

	unsigned int Width;
	unsigned int Height;

	unsigned int NumOfPixels;
public:
	inline Texture()
		: Pixels(nullptr), Width(0), Height(0), NumOfPixels(0) {}

	inline Texture(unsigned int* pixels, unsigned int width, unsigned int height, unsigned int numOfPixels)
		: Pixels(pixels), Width(width), Height(height), NumOfPixels(numOfPixels) { }
};
