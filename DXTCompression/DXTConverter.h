#pragma once
#include "define.h"
#include <iostream>
#include <vector>
using namespace std;

class DXTConverter
{
	struct MemoryDebugStruct
	{
		int bx;
		int by;
		vector<uint8> data;

		bool operator==(const MemoryDebugStruct &other) const {
			
			if (data.size() != other.data.size())
			{
				cout << "Size is not the same at " << bx << " " << by << endl;
				return false;
			}

			for (int i = 0; i < data.size(); i++)
			{
				if ((int)(data[i]) != (int)(other.data[i]))
				{
					cout << "	item not equal at " << bx << " " << by << " at index " << i << endl;
	
					for (int j = 0; j < data.size(); j++)
					{
						cout << (int)(data[j]) << " ";
					}
					cout << endl;

					for (int j = 0; j < other.data.size(); j++)
					{
						cout << (int)(other.data[j]) << " ";
					}
					cout << endl;
					return false;
				}
			}
			return true;
		}
	};

	public:
		void bilinearCompression();
		void compressDXT1(const uint8* srcImgPixels, uint8* dstImgPixels, int w, int h);
		void compressDXT3(const uint8* srcImgPixels, uint8* dstImgPixels, int w, int h);
		void compressDXT5(const uint8* srcImgPixels, uint8* dstImgPixels, int w, int h);



		int getColorDistance(const uint8* color0, const uint8* color1);
		uint16 color888to565(const uint8* color);
		void color565to888(uint16 color, uint8* outputColor);

		void get4x4Block(const uint8* sourceImageStart, int width, int bx, int by, uint8* outputBlock);
		void swapColors(uint8* color0, uint8* color1);
		void getMinMaxColors_EuclideanDistance(uint8* block, uint8* minColor, uint8* maxColor);
		void getMinMaxColors_BoundingBox(uint8* block, uint8* minColor, uint8* maxColor);
		void getMinMaxColorsDXT5_BoundingBox(uint8* block, uint8* minColor, uint8* maxColor);

	//	void build4ColorPalette(uint8* colorPalette, const uint8* minColor, const uint8* maxColor);
		void build4ColorPalette(uint8 colorPalette[][4], const uint8* minColor, const uint8* maxColor);
		void buildAlphaPalette(uint8* alphaPalette, const uint8 minAlpha, const uint8 maxAlpha);
		
	//	void recreateImagePixels(uint8* colorPalette, unsigned int* indices, int bx, int by, int width, int height, uint8* outputImagePixels);
		void recreateBlockInImage(uint8 colorPalette[][4], unsigned int* indices, int bx, int by, int width, int height, uint8* outputImagePixels);
		void writeCompressedAlphaIndices(const uint8* colorBlock, const uint8 minAlpha, const uint8 maxAlpha);
		void writeCompressedIndices(const uint8* colorBlock, const uint8* minColor, const uint8* maxColor);

		void decompressIndices(uint32 compressedIndices, unsigned int* indices);

		void DebugColor(uint8* color);
		void DebugColor565(uint16 color);
		
		// http://www.matejtomcik.com/Public/KnowHow/DXTDecompression/
		void decompressDXT1(const uint8* compressedImagePixels, uint8* dstImgPixels, int width, int height);
		void decompressDXT1Block(uint8* DXTBlock, uint8* outputImagePixels, int bx, int by, int width, int height);

		void decompressDXT3(const uint8* compressedImagePixels, uint8* dstImgPixels, int width, int height);
		void decompressDXT3Block(uint8* DXTBlock, uint8* outputImagePixels, int bx, int by, int width, int height);

		void decompressDXT5(const uint8* compressedImagePixels, uint8* dstImgPixels, int width, int height);
		void decompressDXT5Block(uint8* DXTBlock, uint8* outputImagePixels, int bx, int by, int width, int height);


		void printBlock(uint8* block);
		static void printColor(uint8* color);
		static void printPixel(uint8* image, int pixelStart);
		static int pixelIndex2PixelStart(int width, int px, int py);
		static int blockIndex2PixelStart(int width, int bx, int by, int localBlockX, int localBlockY);

		void setImagePixelColor(uint8* image, int pixelStart, uint8* color);
		void AddToDebugMemoryString(uint8* ptr, int bx, int by, vector<MemoryDebugStruct>& dm);
		void compareDM();

		void setWriteBufferStart(uint8* writeBuffer);

		// assuming little endian
		// http://www.includehelp.com/c-programs/extract-bytes-from-int.aspx
		// https://stackoverflow.com/questions/8680220/how-to-get-the-value-of-individual-bytes-of-a-variable
		// https://stackoverflow.com/questions/34885966/when-an-int-is-cast-to-a-short-and-truncated-how-is-the-new-value-determined
		// so why do we need the 0xFF mask?
		void writeUint8(uint8 value);
		void writeUint16(uint16 value);
		void writeUint32(uint32 value);

		uint8 readUint8();
		uint16 readUint16();
		uint32 readUint32();

	private:
		void printDXTImage(const uint8* dxtImage, int numBlocksX, int numBlocksY);
		void printDXTBlock(const uint8* block);
		uint8* m_internalPtr;

		vector<MemoryDebugStruct> debugMemory;
		vector<MemoryDebugStruct> debugMemory2;

		vector< vector<int> > debugIndicies;

};


