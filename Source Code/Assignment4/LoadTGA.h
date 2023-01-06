#pragma once
#include <fstream>
#include <iostream>

enum ImageType
{
	NoImage = 0,
	UncompressedColorMapped = 1,
	UncompressedRgb = 2,
	UncompressedBlackAndWhite = 3,
	RunLengthColorMapped = 9,
	RunLengthRgb = 10
};

enum FileResult
{
	Ok,
	EndOfFile
};

struct TGA_Header
{
	/* ID Length  -- Length of a string located after the header */
	uint8_t IDLength;
	/* Color Map Type -- Whether a color map is incuded or not
	*	0	-	No Color map data
	*	1	-	Color map data included
	*/
	uint8_t ColorMapType;
	/* Data Type Code
	*	0	-	No Image Data included
	*	1	-	Uncompressed, color mapped images
	*	2	-	Uncompressed, RGB images
	*	3	-	Uncompressed, black and white images
	*	9	-	RunLength encoded color-mapped images
	*	10	-	RunLength encoded RGB images
	*	11	-	Compressed, black and white images
	*	32	-	Compressed, color-mapped data, using Huffman, Delta, and runLength encoding
	*	33	-	Compressed, color-mapped data, using Huffman, Delta, and runlength encoding. 4-pass quadtree-type process
	*/
	uint8_t ImageDataTypeCode;

	/* Color map specification -- describes the color map */
	uint16_t ColorMapOrigin;
	uint16_t ColorMapLength;
	uint8_t ColorMapDepth;

	/* Image Specification -  Image dimensions and format */
	uint16_t X_Origin;
	uint16_t Y_Origin;
	uint16_t Width;
	uint16_t Height;
	uint8_t BitsPerPixel;
	uint8_t ImageDescriptor;
};

FileResult Read8(std::ifstream& fileHandle, uint8_t& outValue)
{
	FileResult result = FileResult::Ok;
	uint8_t value;
	fileHandle.read((char*)(&value), sizeof(uint8_t));
	if (!fileHandle.eof())
	{
		outValue = (uint8_t)value;
		return result;
	}

	result = FileResult::EndOfFile;
	return result;
}

FileResult Read16(std::ifstream& fileHandle, uint16_t& outValue)
{
	FileResult result = FileResult::Ok;
	uint16_t value;
	fileHandle.read((char*)(&value), sizeof(uint16_t));
	if (!fileHandle.eof())
	{
		outValue = (uint16_t)value;
		return result;
	}

	result = FileResult::EndOfFile;
	return result;
}

FileResult Read32(std::ifstream& fileHandle, uint32_t& outValue)
{
	FileResult result = FileResult::Ok;
	uint8_t b1 = 0;
	uint8_t b2 = 0;
	uint8_t b3 = 0;
	uint8_t b4 = 0;
	Read8(fileHandle, b1);
	Read8(fileHandle, b2);
	Read8(fileHandle, b3);
	Read8(fileHandle, b4);

	if (!fileHandle.eof())
	{
		outValue = ((b4 << 24) | (b3 << 16) | (b2 << 8) | b1);
		return result;
	}

	result = FileResult::EndOfFile;
	return result;
}

uint32_t* ReadFile(const char* fileName)
{
	TGA_Header header;

	std::ifstream in(fileName, std::ios::binary);
	if (in.is_open())
	{
		// Read Header
		Read8(in, header.IDLength);
		Read8(in, header.ColorMapType);
		Read8(in, header.ImageDataTypeCode);
		Read16(in, header.ColorMapOrigin);
		Read16(in, header.ColorMapLength);
		Read8(in, header.ColorMapDepth);
		Read16(in, header.X_Origin);
		Read16(in, header.Y_Origin);
		Read16(in, header.Width);
		Read16(in, header.Height);
		Read8(in, header.BitsPerPixel);
		Read8(in, header.ImageDescriptor);

		// Invalid Image Size check
		if (header.Width == 0 || header.Height == 0)
		{
			return nullptr;
		}

		// Skip ID string 
		if (header.IDLength > 0)
		{
			uint8_t i = header.IDLength;
			uint8_t character;
			while (i--)
			{
				if (Read8(in, character) == FileResult::EndOfFile)
				{
					return nullptr;
				}
			}
		}

		uint32_t* pixels = new uint32_t[header.Width * header.Height];

		// Read color map
		if (header.ColorMapType == 1)
		{

		}

		for (int i = 0; i < header.Height; ++i)
		{
			for (int j = 0; j < header.Width; ++j)
			{
				if (Read32(in, pixels[i * header.Width + j]) == FileResult::EndOfFile)
				{
					break;
				}
			}
		}

		in.close();

		return pixels;
	}

	return nullptr;
}
