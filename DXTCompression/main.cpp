
#include <fstream>
#include "define.h"
#include "utility_sdl.h"
#include "DXTConverter.h"
#include <png++/png.hpp>
/* 
links
http://sjbrown.co.uk/2006/01/19/dxt-compression-techniques/


https://software.intel.com/sites/products/vcsource/files/42953/DXTCompressor.pdf

�Real-Time DXT Compression� (Waveren, 2006)
http://www.gamedev.no/projects/MegatextureCompression/324337_324337.pdf

https://www.fsdeveloper.com/wiki/index.php?title=DXT_compression_explained


http://blog.wolfire.com/2009/01/dxtc-texture-compression/


this explains how to do DXT1 with 1 bit alpha channel
https://msdn.microsoft.com/en-us/library/windows/desktop/bb147243(v=vs.85).aspx


used to reduce the amount of GPU memory required for textures
*/
using namespace std;
// http://stackoverflow.com/questions/4845410/error-lnk2019-unresolved-external-symbol-main-referenced-in-function-tmainc
#undef main


string inputPath = "testImages/srcFolder/";
string outputPath = "testImages/myOutput/";

SDL_Surface* createTestImage()
{
	const int sprite_size = 8;

	SDL_Surface *surface;
	Uint32 rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif


	surface = SDL_CreateRGBSurface(0, sprite_size, sprite_size, 32,
		rmask, gmask, bmask, amask);
	if (surface == NULL) {
		cout << SDL_GetError() << endl;
		exit(1);
	}

	int i = 25;
	for (int y = 0; y < sprite_size; y++)
	{
		for (int x = 0; x < sprite_size; x++)
		{
			int pixelIndex = y * sprite_size + x;			
			
			// uint32* ptr = (uint32*)surface->pixels;
			// uint32 color = 0xff00ffff;
			// ptr[pixelIndex] = color;
			// these two are equivalent
				
			uint8* ptr = (uint8*)surface->pixels;
			ptr[pixelIndex * 4 + 0] = 255;	// R
			ptr[pixelIndex * 4 + 1] = i;	// G
			ptr[pixelIndex * 4 + 2] = 0;	// B
			ptr[pixelIndex * 4 + 3] = 255;	// A				
			i++;
		}
	}

	SDL_SaveBMP(surface, "test.bmp");
	return surface;
}




void setImageColor(uint8* image, int pixelStart, uint8* color)
{
	image[pixelStart + 0] = color[0];
	image[pixelStart + 1] = color[1];
	image[pixelStart + 2] = color[2];
	image[pixelStart + 3] = color[3];
}

void setImageAlpha(uint8* image, int pixelStart)
{
	image[pixelStart + 3] = 255;
}


void createImage(string filename, uint8* pixels, int width, int height)
{
	SDL_Surface *image;
	Uint32 rmask, gmask, bmask, amask;
	
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif

	image = SDL_CreateRGBSurface(0, width, height, 32,
		rmask, gmask, bmask, amask);
	if (image == NULL) {
		cout << SDL_GetError() << endl;
		exit(1);
	}

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int ps = DXTConverter::pixelIndex2PixelStart(width, x, y);
			uint8* ptr = (uint8*)image->pixels;
			setImageColor(ptr, ps, &pixels[ps]);
		}
	}

	SDL_SaveBMP(image, filename.c_str());
}

void createPNGImage(string filename, uint8* pixels, int width, int height)
{
	png::image< png::rgba_pixel > image(width, height);

	for (int y = 0; y < image.get_height(); ++y)
	{
		for (int x = 0; x < image.get_width(); ++x)
		{
			int ps = DXTConverter::pixelIndex2PixelStart(width, x, y);
			image[y][x] = png::rgba_pixel(pixels[ps+0], pixels[ps+1], pixels[ps+2], pixels[ps+3]);
		}
	}

	image.write(filename);
}

void copyImageAlphaChannel(string filename, uint8* pixels, int width, int height)
{
	SDL_Surface *image;
	Uint32 rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif

	image = SDL_CreateRGBSurface(0, width, height, 32,
		rmask, gmask, bmask, amask);
	if (image == NULL) {
		cout << SDL_GetError() << endl;
		exit(1);
	}

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int ps = DXTConverter::pixelIndex2PixelStart(width, x, y);
			uint8* ptr = (uint8*)image->pixels;
			ptr[ps + 0] = pixels[ps + 0];
			ptr[ps + 1] = pixels[ps + 1];
			ptr[ps + 2] = pixels[ps + 2];
			ptr[ps + 3] = pixels[ps + 3];

		}
	}

	SDL_SaveBMP(image, filename.c_str());
}


void examineImage(uint8* pixels, int width, int height)
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int ps = DXTConverter::pixelIndex2PixelStart(width, x, y);
			if (pixels[ps + 3] != 255)
			{
				cout << "index " << x << " " << y << " is not 255" << endl;
			}
			else
			{
				cout << "here" << endl;
			}
		}
	}
}



void printImage(uint8* image, int w, int h)
{
	for (int y = 0; y < h; y++)
	{
		if (y >= 8)
		{
			break;
		}

		for (int x = 0; x < w; x++)
		{
			int ps = DXTConverter::pixelIndex2PixelStart(w, x, y);
			DXTConverter::printPixel(image, ps);
			if (x >= 8)
			{
				break;
			}
		}

		cout << endl;
	}
}



void SetImageFullAlpha(uint8* pixels, int width, int height)
{
	SDL_Surface *image;
	Uint32 rmask, gmask, bmask, amask;
	
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif

	image = SDL_CreateRGBSurface(0, width, height, 32,
		rmask, gmask, bmask, amask);
	if (image == NULL) {
		cout << SDL_GetError() << endl;
		exit(1);
	}

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			// uint32* ptr = (uint32*)image->pixels;
			// ptr[y * width + x] = ((uint32*)pixels)[y * width + x];
			

			int ps = DXTConverter::pixelIndex2PixelStart(width, x, y);
			
			uint8* ptr = (uint8*)image->pixels;
		
			// R G B A
			ptr[ps] = pixels[ps];
			ptr[ps + 1] = pixels[ps + 1];
			ptr[ps + 2] = pixels[ps + 2];
			ptr[ps + 3] = pixels[ps + 3];			
		}
	}

	SDL_SaveBMP(image, "lena_full_alpha2.bmp");
}



void testWriteAndReadUint16(uint8* src, int width, int height)
{
	int numBytes = width * height * 4;
	uint8* writeBuffer = new uint8[width * height * 4];
	memset(writeBuffer, 0, numBytes);


	DXTConverter dxtConverter;
	dxtConverter.setWriteBufferStart(writeBuffer);

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int index = (y * width + x) * 2;

			uint16 pixelValue = ((uint16*)src)[index];
			dxtConverter.writeUint16(pixelValue);

			pixelValue = ((uint16*)src)[index + 1];
			dxtConverter.writeUint16(pixelValue);
		}
	}


	uint8* newImageBuffer = new uint8[width * height * 4];

	dxtConverter.setWriteBufferStart(writeBuffer);
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int index = (y * width + x) * 2;

			uint16 pixelValue = dxtConverter.readUint16();
			((uint16*)newImageBuffer)[index] = pixelValue;

			pixelValue = dxtConverter.readUint16();
			((uint16*)newImageBuffer)[index + 1] = pixelValue;
		}
	}


	createImage("testUint16.bmp", newImageBuffer, width, height);

}


void testWriteAndReadUint32(uint8* src, int width, int height)
{
	int numBytes = width * height * 4;

	uint8* writeBuffer = new uint8[width * height * 4];
	memset(writeBuffer, 0, numBytes);


	DXTConverter dxtConverter;
	dxtConverter.setWriteBufferStart(writeBuffer);

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			uint32 pixelValue = ((uint32*)src)[y * width + x];
			dxtConverter.writeUint32(pixelValue);
		}
	}


	uint8* newImageBuffer = new uint8[width * height * 4];

	dxtConverter.setWriteBufferStart(writeBuffer);
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			uint32 pixelValue = dxtConverter.readUint32();
			((uint32*)newImageBuffer)[y * width + x] = pixelValue;
		}
	}


	createImage("testUint32.bmp", newImageBuffer, width, height);
}

void testDXTConveterReadAndWrite(uint8* src, int width, int height)
{
	testWriteAndReadUint16(src, width, height);
	testWriteAndReadUint32(src, width, height);
}




void testIndices(int width, int height)
{
	SDL_Surface *image;
	Uint32 rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif

	image = SDL_CreateRGBSurface(0, width, height, 32,
		rmask, gmask, bmask, amask);
	if (image == NULL) {
		cout << SDL_GetError() << endl;
		exit(1);
	}

	uint8 color[4];
	color[0] = 255;
	color[1] = 255;
	color[2] = 255;
	color[3] = 255;


	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{		
			if (y % 4 == 0)
			{
				int ps = DXTConverter::pixelIndex2PixelStart(width, x, y);
				uint8* ptr = (uint8*)image->pixels;
				setImageColor(ptr, ps, color);
			}
		}
	}


	uint8 color2[4];
	color2[0] = 255;
	color2[1] = 0;
	color2[2] = 0;
	color2[3] = 255;

	vector<int> list = { 1024, 1028, 1032, 1036 };
	uint8* ptr2 = (uint8*)image->pixels;
	for (int i = 0; i < list.size(); i++)
	{
		setImageColor(ptr2, list[i], color2);
	}
	SDL_SaveBMP(image, "testingIndices.bmp");
}


void writeBinFile(string filename, uint8* data, int size)
{
	std::fstream writeFile;
	writeFile = std::fstream(filename.c_str(), std::ios::out | std::ios::binary);
	writeFile.write((char*)data, size);
	writeFile.close();
}

/*
void readBinFile(string filename, char* data)
{
	streampos size;
	ifstream readFile(filename.c_str(), ios::in | ios::binary | ios::ate);
	if (readFile.is_open())
	{
		size = readFile.tellg();
		data = new char[size];
		readFile.seekg(0, ios::beg);
		readFile.read(data, size);
		readFile.close();
	}
}
*/


// DXT1 doesn't work with alpha, cuz you are not compressing the alpha info
// you are compressiong two 888 to 565
// two 16 bit color + 32 bit indicies for each block. So 64 bit for each 4x4 block
// hence u can only assume each block is fully opaque
void runDXT1(string img)
{
	string core = "";

	for (int i = 0; i < img.size() - 4; i++)
	{
		core += img[i];
	}
	cout << core << endl;

	// original image
	string image0Name = img;
	SDL_Surface* image0 = utl::loadSDLImage(inputPath + image0Name);
	createPNGImage(outputPath + image0Name, (uint8*)(image0->pixels), image0->w, image0->h);

	// assuming 8:1 compression ratio
	int numBytes = image0->w * image0->h * 4;
	int numCompressedBytes = numBytes / 8;
	uint8* compressedImage0Pixels = new uint8[numCompressedBytes];
	memset(compressedImage0Pixels, 0, numCompressedBytes);

	DXTConverter dxtConverter;
	dxtConverter.compressDXT1((uint8*)image0->pixels, (uint8*)compressedImage0Pixels, image0->w, image0->h);

	string binFilePath = outputPath + core + "_dxt1.bin";
	writeBinFile(binFilePath, compressedImage0Pixels, numCompressedBytes);

	streampos size;
	char* compressedImageBinaryData = NULL;
	ifstream readFile(binFilePath.c_str(), ios::in | ios::binary | ios::ate);
	if (readFile.is_open())
	{
		size = readFile.tellg();
		compressedImageBinaryData = new char[size];
		readFile.seekg(0, ios::beg);
		readFile.read(compressedImageBinaryData, size);
		readFile.close();
	}
	


	uint8* newImage0Pixels = new uint8[numBytes];
	memset(newImage0Pixels, 0, numBytes);
	dxtConverter.decompressDXT1((uint8*)compressedImageBinaryData, newImage0Pixels, image0->w, image0->h);

	string decompressFileName = core + "_dxt1_decompress.png";
	string decompressFilePath = outputPath + decompressFileName;
//	createImage(decompressFilePath.c_str(), newImage0Pixels, image0->w, image0->h);
	createPNGImage(decompressFilePath.c_str(), newImage0Pixels, image0->w, image0->h);


	cout << "runDXT1 Done " << endl;
}




void runDXT3(string img)
{
	string core = "";

	for (int i = 0; i < img.size() - 4; i++)
	{
		core += img[i];
	}
	cout << core << endl;

	string image0Name = img;
	SDL_Surface* image0 = utl::loadSDLImage(inputPath + image0Name);
	createPNGImage(outputPath + image0Name, (uint8*)(image0->pixels), image0->w, image0->h);


	// assuming 4:1 compression ratio
	int numBytes = image0->w * image0->h * 4;
	int numCompressedBytes = numBytes / 4;
	uint8* compressedImage0Pixels = new uint8[numCompressedBytes];
	memset(compressedImage0Pixels, 0, numCompressedBytes);

	DXTConverter dxtConverter;
	dxtConverter.compressDXT3((uint8*)image0->pixels, (uint8*)compressedImage0Pixels, image0->w, image0->h);

	string binFilePath = outputPath + core + "_dxt3.bin";
	writeBinFile(binFilePath, compressedImage0Pixels, numCompressedBytes);

	streampos size;
	char* compressedImageBinaryData = NULL;
	ifstream readFile(binFilePath.c_str(), ios::in | ios::binary | ios::ate);
	if (readFile.is_open())
	{
		size = readFile.tellg();
		compressedImageBinaryData = new char[size];
		readFile.seekg(0, ios::beg);
		readFile.read(compressedImageBinaryData, size);
		readFile.close();
	}

	uint8* newImage0Pixels = new uint8[numBytes];
	memset(newImage0Pixels, 0, numBytes);
	dxtConverter.decompressDXT3((uint8*)compressedImageBinaryData, newImage0Pixels, image0->w, image0->h);

	string decompressFileName = core + "_dxt3_decompress.png";
	string decompressFilePath = outputPath + decompressFileName;
	createPNGImage(decompressFilePath.c_str(), newImage0Pixels, image0->w, image0->h);

	cout << "testDXT3 Done " << endl;
}


void runDXT5(string img)
{
	string core = "";

	for (int i = 0; i < img.size() - 4; i++)
	{
		core += img[i];
	}
	cout << core << endl;

	string image0Name = img;
	SDL_Surface* image0 = utl::loadSDLImage(inputPath + image0Name);
	createPNGImage(outputPath + image0Name, (uint8*)(image0->pixels), image0->w, image0->h);

	// assuming 4:1 compression ratio
	int numBytes = image0->w * image0->h * 4;
	int numCompressedBytes = numBytes / 4;
	uint8* compressedImage0Pixels = new uint8[numCompressedBytes];
	memset(compressedImage0Pixels, 0, numCompressedBytes);


	uint8* debugPixels = new uint8[numBytes];


	DXTConverter dxtConverter;
	dxtConverter.compressDXT5((uint8*)image0->pixels, (uint8*)compressedImage0Pixels, image0->w, image0->h);



	string binFilePath = outputPath + core + "_dxt5.bin";
	writeBinFile(binFilePath, compressedImage0Pixels, numCompressedBytes);

	streampos size;
	char* compressedImageBinaryData = NULL;
	ifstream readFile(binFilePath.c_str(), ios::in | ios::binary | ios::ate);
	if (readFile.is_open())
	{
		size = readFile.tellg();
		compressedImageBinaryData = new char[size];
		readFile.seekg(0, ios::beg);
		readFile.read(compressedImageBinaryData, size);
		readFile.close();
	}


	uint8* newImage0Pixels = new uint8[numBytes];
	memset(newImage0Pixels, 0, numBytes);
	dxtConverter.decompressDXT5((uint8*)compressedImageBinaryData, newImage0Pixels, image0->w, image0->h);

	string decompressFileName = core + "_dxt5_decompress.png";
	string decompressFilePath = outputPath + decompressFileName;
	createPNGImage(decompressFilePath.c_str(), newImage0Pixels, image0->w, image0->h);

	cout << "testDXT5 Done " << endl;
}

/*

void TestGetAlphaBYBlock(string img)
{

	string core = "";

	for (int i = 0; i < img.size() - 4; i++)
	{
		core += img[i];
	}
	cout << core << endl;


	string image0Name = img;
	SDL_Surface* image0 = utl::loadSDLImage(inputPath + image0Name);


	copyImageAlphaChannel("alphaSample.bmp", (uint8*)image0->pixels, image0->w, image0->h);

	// assuming 8:1 compression ratio
	int numBytes = image0->w * image0->h * 4;
	int numCompressedBytes = numBytes / 2;
	uint8* compressedImage0Pixels = new uint8[numCompressedBytes];
	memset(compressedImage0Pixels, 0, numCompressedBytes);


	uint8* newImage0Pixels = new uint8[numBytes];
	memset(newImage0Pixels, 0, numBytes);

	DXTConverter dxtConverter;
	dxtConverter.DebugCompressTest((uint8*)image0->pixels, (uint8*)compressedImage0Pixels, newImage0Pixels, image0->w, image0->h);
	dxtConverter.DebugDecompressTest((uint8*)compressedImage0Pixels, newImage0Pixels, image0->w, image0->h);

	//	createImage(decompressFilePath.c_str(), newImage0Pixels, image0->w, image0->h);
	createPNGImage("alpha_smokeTest.png", newImage0Pixels, image0->w, image0->h);
//	copyImageAlphaChannel("alpha_smokeTest.bmp", newImage0Pixels, image0->w, image0->h);

	cout << "runDXT1 Done " << endl;
}
*/


int main(int argc, char *argv[])
{	
	SDL_Surface* screen;
	utl::initSDL(SCREEN_WIDTH, SCREEN_HEIGHT, screen);
	
//	TestGetAlphaBYBlock("smoke.png");

	runDXT1("lena.png");
	runDXT3("smoke.png");
	runDXT5("smoke.png");
	/*
	runDXT1("lena.png");

//	runDXT1("smoke.png");
	runDXT3("smoke.png");
//	runDXT5("smoke.png");
	*/
	while (1)
	{}
	return 0;
}






