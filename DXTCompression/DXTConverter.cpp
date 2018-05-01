#include "DXTConverter.h"

#include <bitset>

int targetX = 0;
int targetY = 1;

/*
width and height are in pixels, not in bytes
*/


void DXTConverter::DebugColor(uint8* color)
{
	/*
	uint32 value = *((uint32*)color);

	printColor(color);
	std::bitset<32> bitColor(value);
	cout << bitColor << endl;

	uint16 color565 = color888to565(color);
	std::bitset<16> bitColor2(color565);
	cout << bitColor2 << endl;
	*/


	printColor(color);

	uint8 a0 = color[0];
	uint8 a1 = color[1];
	uint8 a2 = color[2];
	uint8 a3 = color[3];

	std::bitset<8> a00(a0);
	std::bitset<8> a11(a1);
	std::bitset<8> a22(a2);
	std::bitset<8> a33(a3);

	cout << a00 << " " << a11 << " " << a22 << " " << a33 << endl << endl;
}


void DXTConverter::DebugColor565(uint16 color565)
{
//	uint16 color565 = color888to565(color);
	uint8* temp = ((uint8*)(&color565));
	std::bitset<16> bitColor2(color565);


	uint8 r = (color565 & C565_R_MASK) >> 11;
	uint8 g = (color565 & C565_G_MASK) >> 5;
	uint8 b = (color565 & C565_B_MASK);

	std::bitset<5> b00(r);
	std::bitset<6> b11(g);
	std::bitset<5> b22(b);

	cout << b22 << " " << b11 << " " << b00 << endl << endl << endl;
}

void DXTConverter::compressImageDXT1(const uint8* sourceImagePixels,
	uint8* outputImagePixels, int width, int height, int &outputBytes)
{
	// 4x4 pixels, each pixel is 4 bytes
	// hence 64 bytes
	uint8 block4x4[64];
	uint8 minColor[4];
	uint8 maxColor[4];

	m_internalPtr = outputImagePixels;

	// every 4x4 block

	int numBlocksX = width / 4;
	int numBlocksY = height / 4;

	for (int by = 0; by < numBlocksY; by++)
	{
		for (int bx = 0; bx < numBlocksX; bx++)
		{
			// 16 pixels in a block, 4 bytes in a pixel
		//	int blockIndex = (y * numBlocksX + x);
		//	int startIndex = blockIndex * 16 * 4;	
			get4x4Block(sourceImagePixels, width, bx, by, block4x4);

//			getMinMaxColors_BoundingBox(block4x4, minColor, maxColor);
			getMinMaxColors_EuclideanDistance(block4x4, minColor, maxColor);

			
			if (bx == targetX && by == targetY)
			{
				cout << endl << "Printing block " << bx << " " << by << endl;
				printBlock(block4x4);
				cout << endl << "	Priting minColor, maxColor" << endl;

				DebugColor(minColor);	DebugColor565(color888to565(minColor));
				DebugColor(maxColor);	DebugColor565(color888to565(maxColor));
			}
			
			uint16 compressedMaxColor = color888to565(maxColor);
			uint16 compressedMinColor = color888to565(minColor);
			

			write(compressedMaxColor);
			write(compressedMinColor);

			uint32 indices = getCompressedIndices(block4x4, minColor, maxColor);
			write(indices);
		}
	}

	outputBytes = m_internalPtr - outputImagePixels;
	cout << "outputBytes " << outputBytes << endl;
}



void DXTConverter::printBlock(uint8* block)
{
	for (int i = 0; i < 16; i++)
	{
		if (i > 0 && i % 4 == 0)
		{
			cout << endl;
		}
		printPixel(block, i*4);
	}
}

void DXTConverter::printColor(uint8* color)
{
	DXTConverter::printPixel(color, 0);
	cout << endl;
}



int DXTConverter::getColorDistance(const uint8* color0, const uint8* color1)
{
	// little endian
	int rdiff = color0[3] - color1[3];
	int gdiff = color0[2] - color1[2];
	int bdiff = color0[1] - color1[1];

	/*
	cout << (int)color0[0] << " " << (int)color1[0] << endl;
	cout << (int)color0[1] << " " << (int)color1[1] << endl;
	cout << (int)color0[2] << " " << (int)color1[2] << endl;
	*/

	return rdiff * rdiff + gdiff * gdiff + bdiff * bdiff;
}


// 16-bit RGB
// https://en.wikipedia.org/wiki/List_of_monochrome_and_RGB_palettes#16-bit_RGB
// "Usually, there are 5 bits allocated for the red and blue color components
//	and 6 bits for the green component, due to the greater sensitivity of the common human
//	eye to this color. 
uint16 DXTConverter::color888to565(const uint8* color)
{
	uint16 r = (color[3] >> 3) << 11;
	uint16 g = (color[2] >> 2) << 5;
	uint16 b = (color[1] >> 3);
	return r | g | b;
}


void DXTConverter::color565to888(uint16 color, uint8* outputColor)
{
	uint8 r = ((color & C565_R_MASK) >> 11) & 0xFF;
	r = ((r << 3) | (r >> 2));

	uint8 g = ((color & C565_G_MASK) >> 5) & 0xFF;
	g = ((g << 2) | (g >> 4));

	uint8 b = ((color & C565_B_MASK)) & 0xFF;
	b = ((b << 3) | (b >> 2));

	outputColor[0] = 255;
	outputColor[1] = b;
	outputColor[2] = g;
	outputColor[3] = r;
}




// assuming little endian
void DXTConverter::write(uint16 value)
{
	m_internalPtr[0] = (value >> 0) & 0xFF;
	m_internalPtr[1] = (value >> 8) & 0xFF;
	m_internalPtr += 2;
}

// http://www.includehelp.com/c-programs/extract-bytes-from-int.aspx
// https://stackoverflow.com/questions/8680220/how-to-get-the-value-of-individual-bytes-of-a-variable
// https://stackoverflow.com/questions/34885966/when-an-int-is-cast-to-a-short-and-truncated-how-is-the-new-value-determined
// so why do we need the 0xFF mask?
void DXTConverter::write(uint32 value)
{
	m_internalPtr[0] = (value >> 0) & 0xFF;
	m_internalPtr[1] = (value >> 8) & 0xFF;
	m_internalPtr[2] = (value >> 16) & 0xFF;
	m_internalPtr[3] = (value >> 24) & 0xFF;

	m_internalPtr += 4;
}




/*
we copy the 4x4 block to our outputBlock

_ _ _ _		row0	0 - 15
_ _ _ _		row1	16 - 31
_ _ _ _		row2	32 - 47
_ _ _ _		row3	48 - 63


*/
void DXTConverter::get4x4Block(const uint8* sourceImageStart, int width, int bx, int by, uint8* outputBlock)
{
	for (int y = 0; y < 4; y++)
	{
		int pixelStart = blockIndex2PixelStart(width, bx, by, y * 4);

		memcpy(&outputBlock[y * 4 * 4], &sourceImageStart[pixelStart], 4 * 4);
	}
}





void DXTConverter::swapColors(uint8* color0, uint8* color1)
{
	uint8 temp[4];
	memcpy(temp, color0, 4);
	memcpy(color0, color1, 4);
	memcpy(color1, temp, 4);
}


/*
finds the two end points of the line through color space
section 2.1:
uuse the two colors from the 4x4 block that are furthest apart as the end points of
the line through color space.

I am assuming this is RGB color space?
*/
void DXTConverter::getMinMaxColors_EuclideanDistance(uint8* block, uint8* minColor, uint8* maxColor)
{
	int maxDistance = -1;

	int numPixels = 64 / NUM_BYTES_IN_PIXEL;

	for (int i = 0; i < numPixels - 1; i++)
	{
		for (int j = i + 1; j < numPixels; j++)
		{
			uint8* color0 = &block[i*NUM_BYTES_IN_PIXEL];
			uint8* color1 = &block[j*NUM_BYTES_IN_PIXEL];

		//	printColor(color0);
		//	printColor(color1);

			int distance = getColorDistance(color0, color1);

			if (distance > maxDistance)
			{
				maxDistance = distance;
				memcpy(minColor, block + i * NUM_BYTES_IN_PIXEL, 4);
				memcpy(maxColor, block + j * NUM_BYTES_IN_PIXEL, 4);
			}
		}
	}

	if (color888to565(maxColor) < color888to565(minColor))
	{
		swapColors(minColor, maxColor);
	}
}



void DXTConverter::getMinMaxColors_BoundingBox(uint8* block, uint8* minColor, uint8* maxColor)
{
	uint8 inset[3];

	minColor[0] = minColor[1] = minColor[2] = 255;
	maxColor[0] = maxColor[1] = maxColor[2] = 255;

	int numPixels = 16;

	for (int i = 0; i < numPixels; i++)
	{
		int pixelIndex = i * NUM_BYTES_IN_PIXEL;
		for (int j = 0; j < 3; j++)
		{
			if (block[pixelIndex + j] < minColor[j])
				minColor[j] = block[pixelIndex + j];

			if (block[pixelIndex + j] > maxColor[j])
				maxColor[j] = block[pixelIndex + j];
		}
	}

	for (int i = 0; i < 3; i++)
	{
		inset[i] = (maxColor[i] - minColor[i]) >> INSET_SHIFT;

		// if all white, we are clamping it minColor
		uint8 newMinColor = minColor[i] + inset[i];
		minColor[i] = (newMinColor <= 255) ? newMinColor : 255;

		// if all black, we are clamping it for maxColor
		uint8 newMaxColor = maxColor[i] - inset[i];
		maxColor[i] = (newMaxColor >= 0) ? newMaxColor : 0;
	}
}

// When compressing, a 4 color palette is determined for these 16 pixels
// https://www.fsdeveloper.com/wiki/index.php?title=DXT_compression_explained#DXT1

// we are replicating the high order bits of the minColor and maxColor to the low order bits
// the same way the graphics card converts the 16-bit 5:6:5 RGB format to 24-bit 8:8:8 RGB format
// so for R, assuming our five bits are 11011, we will get 11011_110	after replicating the top 3 bits to the lower end
// same thing for B. 
// for G, we do the samething for 6 bits
// so if we have 111001, we have 111001_11

void DXTConverter::build4ColorPalette(uint8* colorPalette, const uint8* minColor, const uint8* maxColor)
{
	// the order here doesn't matter, cuz we will be comparing each color in the colorBlock
	// with each color in the palette

	for (int i = 0; i < 4; i++)
	{
		if (i == 0)
		{
			colorPalette[0 + i] = 255;
			colorPalette[4 + i] = 255;
			colorPalette[8 + i] = 255;
			colorPalette[12 + i] = 255;
		}
		else
		{
			if (i == 2)
			{
				colorPalette[0 + i] = (maxColor[i] & C565_6_MASK) | (maxColor[i] >> 6);
				colorPalette[4 + i] = (minColor[i] & C565_6_MASK) | (minColor[i] >> 6);
			}
			else
			{
				colorPalette[0 + i] = (maxColor[i] & C565_5_MASK) | (maxColor[i] >> 5);
				colorPalette[4 + i] = (minColor[i] & C565_5_MASK) | (minColor[i] >> 5);
			}

			// interpolate the other two points 
			colorPalette[8 + i] = (2 * colorPalette[i] + 1 * colorPalette[4 + i]) / 3;
			colorPalette[12 + i] = (1 * colorPalette[i] + 2 * colorPalette[4 + i]) / 3;
		}
	}
}


// the color vlaues of pixels in a 4x4 pixel block are approximated with equidistant points on a line 
// through RGB color space. 
// for each pixel in the 4x4 block, a 2-bit index is stored to one of the equidistanct points
// on the line. 

// The end-points of the line through color space are quantized to a 16-bit 5:6:5 RGB format
// and either one or two intermediate points are generated through interpolation. 
// http://www.nvidia.com/object/real-time-ycocg-dxt-compression.html

// afterwards each pixel will get an index into this palette, which only require 2 bit per pixel
// For the palette only two colours are stored, the two extermes, and the other two colours are interpolated between 
// these exteremes. 

// so for the palette we have 32 bits: two extremes, each with stored as 565
// then for the colorBlock, each pixel stores an index: so 16 * 2 = 32 bits
// so we just need 64 bits for each color block
// previously, we needed 16 pixels. each pixel is 32 bits: so 16 * 32 = 512
// hence a 8:1 compression ratio
uint32 DXTConverter::getCompressedIndices(const uint8* block4x4, const uint8* minColor, const uint8* maxColor)
{

	// we are getting four points
	uint8 colorPalette[4 * 4];
	build4ColorPalette(colorPalette, minColor, maxColor);

	// this is the indices into the palette;
	unsigned int indices[NUM_PIXELS_IN_BLOCK];

	for (int i = 0; i < NUM_PIXELS_IN_BLOCK; i++)
	{
		unsigned int minDistance = INT_MAX;

		// we compare with all the colors in the palette
		for (int j = 0; j < 4; j++)
		{
			unsigned int dist = getColorDistance(&block4x4[i * 4], &colorPalette[j * 4]);

			if (dist < minDistance)
			{
				minDistance = dist;
				indices[i] = j;
			}
		}
	}

	uint32 result = 0;
	for (unsigned int i = 0; i < 16; i++)
	{
	//	cout << indices[i] << " ";
		unsigned int numBitsToShift = i << 1;  // multiply by 2
		result |= (indices[i] << numBitsToShift);
	}
	// cout << endl;
	return result;
}


void DXTConverter::decompress(const uint8* compressedImagePixels, uint8* outputImagePixels, int width, int height)
{
	// each original block is 4x4 pixels
	// 16 * 4 bytes = 64 bytes
	
	// each compressed block is 64 bit
	// 32 bits: 2 x 565 for the two colors
	// 32 bits: 16 x 2 bit indices
	// so 8 bytes

	// so it's a 8:1 compression ratio

	cout << ">>>>>>>>>>>> Decompressing " << endl;


	uint8 compressedBlock[8];

	m_internalPtr = outputImagePixels;

	int numBlocksX = width / 4;
	int numBlocksY = height / 4;

	cout << "numBlocksX " << numBlocksX << ", numBlocksY " << numBlocksY << endl;

//	printDXTImage(compressedImagePixels, numBlocksX, numBlocksY);
	
	// every 4x4 block
	for (int y = 0; y < numBlocksY; y++)
	{
		for (int x = 0; x < numBlocksX; x++)
		{
			int startIndex = (y * numBlocksX + x) * 8;
//			cout << "startIndex " << startIndex << ", x " << x << ", y " << y << endl;
			// 0, 51 giving me error
//			readCompressed4x4Block(&compressedImagePixels[startIndex], compressedBlock, startIndex, x, y);

			readCompressed4x4Block(compressedImagePixels, compressedBlock, startIndex, x, y);

			decompressDXTBlock(compressedBlock, outputImagePixels, x, y, width, height);			
		}

		// width * 4 is num of pixels in a 4x4 row
		// 4 is numBytes in each pixel
		int numBytesIn4x4BlockRow = width * 4 * 4;

		compressedImagePixels += numBytesIn4x4BlockRow;
	}
	
	
}


void DXTConverter::readCompressed4x4Block(const uint8* source, uint8* outputBlock, int startIndex, int x, int y)
{
	// we are reading 64 bit;
	for (int i = 0; i < 8; i++)
	{
		outputBlock[i] = source[startIndex + i];
	}
}


void decompressIndices(uint32 compressedIndices, unsigned int* indices);

void DXTConverter::decompressDXTBlock(uint8* DXTBlock, uint8* outputImagePixels, int bx, int by, int width, int height)
{


	// we are getting four points
	uint8 colorPalette[4 * 4];
	uint8 maxColor[4];
	uint8 minColor[4];

	uint8* temp = DXTBlock;

	m_internalPtr = DXTBlock;
	uint16 compressedMaxColor = readUint16();
	uint16 compressedMinColor = readUint16();



	color565to888(compressedMaxColor, maxColor);
	color565to888(compressedMinColor, minColor);

	if (bx == targetX && by == targetY)
	{
		DebugColor(minColor);	DebugColor565(compressedMinColor);
		DebugColor(maxColor);	DebugColor565(compressedMaxColor);

		int a = 1;
	}

	build4ColorPalette(colorPalette, minColor, maxColor);

	unsigned int indices[NUM_PIXELS_IN_BLOCK];
	uint32 compressedIndices = readUint32();
	decompressIndices(compressedIndices, indices);

	recreateImagePixels(colorPalette, indices, bx, by, width, height, outputImagePixels);
}




uint16 DXTConverter::readUint16()
{
	uint16 result = 0;
	result = m_internalPtr[0] | (m_internalPtr[1] << 8);

	m_internalPtr += 2;
	return result;
}


uint32 DXTConverter::readUint32()
{	
	uint32 result = 0;
	result = (m_internalPtr[0]) | (m_internalPtr[1] << 8) | (m_internalPtr[2] << 16) | (m_internalPtr[3] << 24);

	m_internalPtr += 4;
	return result;
}

void DXTConverter::decompressIndices(uint32 compressedIndices, unsigned int* indices)
{
	for (unsigned int i = 0; i < 16; i++)
	{
		unsigned int numBitsToShift = i << 1;  // multiply by 2
//		result |= (indices[i] << numBitsToShift);
		unsigned int mask = 3 << numBitsToShift;

		unsigned int index = (compressedIndices & mask) >> numBitsToShift;
		indices[i] = index;
	}
}




/*
	_ _ _ _
	_ _ _ _
	_ _ _ _
	_ _ _ _
*/

// which block r u in
// indexInBlock: should be 0 ~ 15
int DXTConverter::blockIndex2PixelStart(int width, int bx, int by, int indexInBlock)
{
	int numBlocksX = width / 4;
	int bi = (by * numBlocksX + bx);
	int firstPixelStart = bi * 64;


	int y = indexInBlock / 4;
	int x = indexInBlock % 4;


	// width:		num a pixels in a row
	// width * 4:	numBytes in a row
	return firstPixelStart + y * width * 4 + x * 4;
}


int DXTConverter::pixelIndex2PixelStart(int width, int px, int py)
{
	return (py * width + px) * 4;
}

void DXTConverter::printPixel(uint8* image, int pixelStart)
{
	cout << (int)(image[pixelStart + 0]) << " " << (int)(image[pixelStart + 1]) << " "
		<< (int)(image[pixelStart + 2]) << " " << (int)(image[pixelStart + 3]) << ", ";
}

void DXTConverter::setImageColor(uint8* image, int pixelStart, uint8* color)
{
	image[pixelStart + 0] = color[0];
	image[pixelStart + 1] = color[1];
	image[pixelStart + 2] = color[2];
	image[pixelStart + 3] = color[3];
}


void DXTConverter::recreateImagePixels(uint8* colorPalette, unsigned int* indices, int bx, int by, int width, int height, uint8* outputImagePixels)
{
	int numBlocksX = width / 4;
	int numBlocksY = height / 4;
	int pixelIndex = (by * numBlocksX + bx) * 64;

	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 4; x++)
		{
			int pixelIndex = blockIndex2PixelStart(width, bx, by, y * 4 + x);

			int colorIndex = indices[y * 4 + x];
			
			setImageColor(outputImagePixels, pixelIndex, &colorPalette[colorIndex]);
		}

	}
}


void DXTConverter::printDXTImage(const uint8* dxtImage, int numBlocksX, int numBlocksY)
{
	uint8 block[8];

	for (int y = 0; y < numBlocksY; y++)
	{
		/*
		if (y >= 8)
		{
			break;
		}
		*/

		for (int x = 0; x < numBlocksX; x++)
		{
			int index = (y * numBlocksX + x) * 8;
	
			if (x == 0 && y == 51)
			{
				cout << "index " << index << endl;
				for (int i = 0; i < 8; i++)
				{
					cout << (int)dxtImage[index + i] << " ";
					block[i] = dxtImage[index + i];
				}

				cout << endl;
			}
			printDXTBlock(block);
			
			/*
			if (x >= 8)
			{
				break;
			}
			*/
		}

	//	cout << endl;
	}
}

void DXTConverter::printDXTBlock(const uint8* block)
{
	

//	cout << (int)(block[0]) << " " << (int)(block[1]) << " " << (int)(block[ndex + 2]) << " " << (int)(ptr[index + 3]) << ",	";

}

