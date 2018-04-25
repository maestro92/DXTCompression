#include "utility.h"
#include "utility_sdl.h"
/* 
links
http://sjbrown.co.uk/2006/01/19/dxt-compression-techniques/


https://software.intel.com/sites/products/vcsource/files/42953/DXTCompressor.pdf

“Real-Time DXT Compression” (Waveren, 2006)
http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.215.7942&rep=rep1&type=pdf

*/
using namespace std;
// http://stackoverflow.com/questions/4845410/error-lnk2019-unresolved-external-symbol-main-referenced-in-function-tmainc
#undef main

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

#define NUM_BYTES_IN_PIXEL		4
#define NUM_PIXELS_IN_BLOCK		16

// inset the bounding box with 1/16th of its size
// (inset means shrink, we are shrinking the bounding box by 1/16th of its size)
#define INSET_SHIFT		4

#define C565_5_MASK		0xF8	// 1111 1000,	minus the last three bits
#define C565_6_MASK		0xFC	// 1111 1100	minus the last two bits

uint8* globalOutData;


void RotateAndSaveImg(string filename) {
	const int sprite_size = 200;
	/*
	const int sprite_size = 75;
	SDL_Surface *surface =
		SDL_CreateRGBSurface(SDL_SWSURFACE, 360 * sprite_size, sprite_size, 32, 0x000000ff,
			0x0000ff00, 0x00ff0000, 0xff000000);

	SDL_Surface *rotated_picture;
	//rotated_picture =	rotozoomSurface(image, (double)angle, 1, smooth);
	*/
	SDL_Surface *surface;
	Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xffffffff;
	gmask = 0xffffffff;
	bmask = 0xffffffff;
	amask = 0xffffffff;
#else
	rmask = 0xffffffff;
	gmask = 0xffffffff;
	bmask = 0xffffffff;
	amask = 0xffffffff;
#endif

	surface = SDL_CreateRGBSurface(0, sprite_size, sprite_size, 32,
		rmask, gmask, bmask, amask);
	if (surface == NULL) {
		cout << SDL_GetError() << endl;
		exit(1);
	}


	SDL_SaveBMP(surface, filename.c_str());
}


/*
we copy the 4x4 block to our outputBlock
						
	_ _ _ _		row0	0 - 15
	_ _ _ _		row1	16 - 31
	_ _ _ _		row2	32 - 47
	_ _ _ _		row3	48 - 63


*/
void extractBlock(const uint8* sourceImageStart, int width, uint8* outputBlock)
{
	for (int y = 0; y < 4; y++)
	{
		memcpy(&outputBlock[y * 4 * 4], sourceImageStart, 4 * 4);

		// we get to the next row, so it's width * 4 
		// (width number of pixels, each pixels is 4 bytes)
		sourceImageStart += width * 4;
	}
}



int getColorDistance(const uint8* color0, const uint8* color1)
{
	int rdiff = color0[0] - color1[0];
	int gdiff = color0[1] - color1[1];
	int bdiff = color0[2] - color1[2];
	
	return rdiff * rdiff + gdiff * gdiff + bdiff * bdiff;
}


void swapColors(uint8* color0, uint8* color1)
{
	uint8 temp[3];
	memcpy(temp, color0, 3);
	memcpy(color0, color1, 3);
	memcpy(color1, temp, 3);
}


/*
	finds the two end points of the line through color space
section 2.1: 
	uuse the two colors from the 4x4 block that are furthest apart as the end points of
	the line through color space. 
	
		I am assuming this is RGB color space?	
*/
void getMinMaxColors_EuclideanDistance(uint8* block, uint8* minColor, uint8* maxColor)
{
	int maxDistance = -1;
	
	int numPixels = 64 / NUM_BYTES_IN_PIXEL;

	for (int i = 0; i < numPixels - 1; i++)
	{
		for (int j = i + 1; j < numPixels; j++)
		{
			uint8* color0 = &block[i*NUM_BYTES_IN_PIXEL];
			uint8* color1 = &block[j*NUM_BYTES_IN_PIXEL];

			int distance = getColorDistance(color0, color1);

			if (distance > maxDistance)
			{
				maxDistance = distance;
				memcpy(minColor, block+i*NUM_BYTES_IN_PIXEL, 4);
				memcpy(maxColor, block+j*NUM_BYTES_IN_PIXEL, 4);
			}
		}
	}

	if (color888to565(maxColor) < color888to565(minColor))
	{
		swapColors(minColor, maxColor);
	}
}



void getMinMaxColors_BoundingBox(uint8* block, uint8* minColor, uint8* maxColor)
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

void Compute4ColorPalette(uint8* colorPalette, const uint8* minColor, const uint8* maxColor)
{
	// the order here doesn't matter, cuz we will be comparing each color in the colorBlock
	// with each color in the palette

	for (int i = 0; i < 3; i++)
	{
		if (i != 1)
		{
			colorPalette[0 + i] = (maxColor[i] & C565_5_MASK) | (maxColor[i] >> 5);
			colorPalette[4 + i] = (minColor[i] & C565_5_MASK) | (minColor[i] >> 5);
		}
		else
		{
			colorPalette[0 + i] = (maxColor[i] & C565_6_MASK) | (maxColor[i] >> 6);
			colorPalette[4 + i] = (minColor[i] & C565_6_MASK) | (minColor[i] >> 6);
		}

		// interpolate the other two points 
		colorPalette[8 + i] =  (2 * colorPalette[i]	+ 1 * colorPalette[4 + i]) / 3;
		colorPalette[12 + i] = (1 * colorPalette[i]	+ 2 * colorPalette[4 + i]) / 3;
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
uint32 ComputeColorIndicesForPixels(const uint8* colorBlock, const uint8* minColor, const uint8* maxColor)
{

	// we are getting four points
	uint8 colorPalette[4 * 4];
	Compute4ColorPalette(colorPalette, minColor, maxColor);

	// this is the indices into the palette;
	unsigned int indices[NUM_PIXELS_IN_BLOCK];

	for (int i = 0; i < NUM_PIXELS_IN_BLOCK; i++)
	{
		unsigned int minDistance = INT_MAX;

		// we compare with all the colors in the palette
		for (int j = 0; j < 4; j++)
		{
			unsigned int dist = getColorDistance(&colorBlock[i * 4], &colorPalette[j * 4]);

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
		unsigned int numBitsToShift = i << 1;  // multiply by 2
		result |= (indices[i] << numBitsToShift);
	}
	return result;
}

// assuming little endian
void write(uint16 value)
{
	globalOutData[0] = (value >> 0) & 0xFF;
	globalOutData[1] = (value >> 8) & 0xFF;
	globalOutData += 2;
}

// http://www.includehelp.com/c-programs/extract-bytes-from-int.aspx
// https://stackoverflow.com/questions/8680220/how-to-get-the-value-of-individual-bytes-of-a-variable
// https://stackoverflow.com/questions/34885966/when-an-int-is-cast-to-a-short-and-truncated-how-is-the-new-value-determined
// so why do we need the 0xFF mask?
void write(uint32 value)
{
	globalOutData[0] = (value >> 0) & 0xFF;
	globalOutData[1] = (value >> 8) & 0xFF;
	globalOutData[2] = (value >> 16) & 0xFF;
	globalOutData[3] = (value >> 24) & 0xFF;

	globalOutData += 4;
}



// 16-bit RGB
// https://en.wikipedia.org/wiki/List_of_monochrome_and_RGB_palettes#16-bit_RGB
// "Usually, there are 5 bits allocated for the red and blue color components
//	and 6 bits for the green component, due to the greater sensitivity of the common human
//	eye to this color. 
uint16 color888to565(const uint8* color)
{
	uint16 r = (color[0] >> 3) << 11;
	uint16 g = (color[1] >> 2) << 5;
	uint16 b = (color[2] >> 3);
	return color[0] | color[1] | color[2];
}


/*
width and height are in pixels, not in bytes
*/
void compressImageDXT1(const uint8* sourceImagePixels,
							uint8* outputImagePixels, int width, int height,
						int &outputBytes)
{
	// 4x4 pixels, each pixel is 4 bytes
	// hence 64 bytes
	uint8 block4x4[64];
	uint8 minColor[4];
	uint8 maxColor[4];

	globalOutData = outputImagePixels;

	// every 4x4 block
	for (int y = 0; y < height; y += NUM_BYTES_IN_PIXEL)
	{
		for (int x = 0; x < width; x += NUM_BYTES_IN_PIXEL)
		{
			extractBlock(sourceImagePixels, width, block4x4);
			getMinMaxColors_BoundingBox(block4x4, minColor, maxColor);

			write(color888to565(maxColor));
			write(color888to565(minColor));

			uint32 indicies = ComputeColorIndicesForPixels(block4x4, minColor, maxColor);
			write(indicies);
		}

		// width * 4 is num of pixels in a 4x4 row
		// 4 is numBytes in each pixel
		int numBytesIn4x4BlockRow = width * 4 * 4;

		sourceImagePixels += numBytesIn4x4BlockRow;
	}

	outputBytes = globalOutData - outputImagePixels;
}








int main(int argc, char *argv[])
{
	SDL_Surface* screen;
	utl::initSDL(utl::SCREEN_WIDTH, utl::SCREEN_HEIGHT, screen);

	//Load image
	SDL_Surface* hello = utl::loadSDLImage("icon_armor.png");

	short int x = 200;
	short int y = 350;

	for (int x = 0; x < 20; x++)
	{
		for (int y = 0; y < 20; y++)
		{
			unsigned char* pixels = (unsigned char*)hello->pixels;
			pixels[4 * (y * hello->w + x) + 0] = 0;
		}
	}




	SDL_SaveBMP(hello, "nice3.bmp");
	RotateAndSaveImg("nice2.bmp");

	//Apply image to screen
	SDL_BlitSurface(hello, NULL, screen, NULL);

	//Update Screen
	SDL_Flip(screen);

	//Pause
	SDL_Delay(2000);



	while (1)
	{
	}
	SDL_LockSurface(screen);

	SDL_UnlockSurface(screen);


	/*
	//...
	png::image< png::rgb_pixel > image(128, 128);
	for (size_t y = 0; y < image.get_height(); ++y)
	{
		for (size_t x = 0; x < image.get_width(); ++x)
		{
			image[y][x] = png::rgb_pixel(x, y, x + y);
			// non-checking equivalent of image.set_pixel(x, y, ...);
		}
	}
	image.write("rgb.png");
	*/
	return 0;
}















// http://www.matejtomcik.com/Public/KnowHow/DXTDecompression/
void Decompress()
{

}