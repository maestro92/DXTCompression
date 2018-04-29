#pragma once
#include "define.h"
#include <iostream>
using namespace std;

class DXTConverter
{
	public:
		void compressImageDXT1(const uint8* sourceImagePixels, uint8* outputImagePixels, int width, int height, int &outputBytes);
		int getColorDistance(const uint8* color0, const uint8* color1);
		uint16 color888to565(const uint8* color);
		void color565to888(uint16 color, uint8* outputColor);

		void get4x4Block(const uint8* sourceImageStart, int width, int bx, int by, uint8* outputBlock);
		void swapColors(uint8* color0, uint8* color1);
		void getMinMaxColors_EuclideanDistance(uint8* block, uint8* minColor, uint8* maxColor);
		void getMinMaxColors_BoundingBox(uint8* block, uint8* minColor, uint8* maxColor);
		void build4ColorPalette(uint8* colorPalette, const uint8* minColor, const uint8* maxColor);
		uint32 getCompressedIndices(const uint8* colorBlock, const uint8* minColor, const uint8* maxColor);

		void decompressIndices(uint32 compressedIndices, unsigned int* indices);


		// http://www.matejtomcik.com/Public/KnowHow/DXTDecompression/
		void decompress(const uint8* sourceImagePixels, uint8* outputImagePixels, int width, int height);
		void decompressDXTBlock(uint8* DXTBlock, uint8* outputImagePixels, int bx, int by, int width, int height);
		void recreateImagePixels(uint8* colorPalette, unsigned int* indices, int bx, int by, int width, int height, uint8* outputImagePixels);
		void recreateBlock(uint8* colorPalette, unsigned int* indices, int bx, int by, int width, int height, uint8* outputImagePixels);

		void printBlock(uint8* block);
		static void printColor(uint8* color);
		static void printPixel(uint8* image, int pixelStart);
		static int pixelIndex2PixelStart(int width, int px, int py);
		static int blockIndex2PixelStart(int width, int bx, int by, int indexInBlock);

		void setImageColor(uint8* image, int pixelStart, uint8* color);

private:
		// assuming little endian
		void write(uint16 value);
		// http://www.includehelp.com/c-programs/extract-bytes-from-int.aspx
		// https://stackoverflow.com/questions/8680220/how-to-get-the-value-of-individual-bytes-of-a-variable
		// https://stackoverflow.com/questions/34885966/when-an-int-is-cast-to-a-short-and-truncated-how-is-the-new-value-determined
		// so why do we need the 0xFF mask?
		void write(uint32 value);

//		void readCompressed4x4Block(const uint8* sourceImageStart, uint8* outputBlock);
		void readCompressed4x4Block(const uint8* sourceImageStart, uint8* outputBlock, int startIndex, int x, int y);
		void printDXTImage(const uint8* dxtImage, int numBlocksX, int numBlocksY);
		void printDXTBlock(const uint8* block);
		//	void readColor(uint8* color);
	//	void readIndices(uint8* color);

		uint16 readUint16();
		uint32 readUint32();


		uint8* m_internalPtr;
};


