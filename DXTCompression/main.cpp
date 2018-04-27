

#include "define.h"
#include "utility_sdl.h"
#include "DXTConverter.h"
/* 
links
http://sjbrown.co.uk/2006/01/19/dxt-compression-techniques/


https://software.intel.com/sites/products/vcsource/files/42953/DXTCompressor.pdf

“Real-Time DXT Compression” (Waveren, 2006)
http://www.gamedev.no/projects/MegatextureCompression/324337_324337.pdf

*/
using namespace std;
// http://stackoverflow.com/questions/4845410/error-lnk2019-unresolved-external-symbol-main-referenced-in-function-tmainc
#undef main








SDL_Surface* createTestImage()
{
	const int sprite_size = 256;

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

	int i = 0;
	for (int y = 0; y < sprite_size; y++)
	{
		for (int x = 0; x < sprite_size; x++)
		{
			i = 150;
			int pixelIndex = y * sprite_size + x;
			uint32* ptr = (uint32*)surface->pixels;

			if (x < 100)
			{
				uint32 color = 0xff777777;
				ptr[pixelIndex] = color;
				// these two are equivalent
				/*
				ptr[pixelIndex * 4 + 0] = i;
				ptr[pixelIndex * 4 + 1] = i;
				ptr[pixelIndex * 4 + 2] = i;
				ptr[pixelIndex * 4 + 3] = 255;
				*/
			}
			else
			{
				uint32 color = 0xff00ffff;
				ptr[pixelIndex] = color;
				// these two are equivalent
				/*
				ptr[pixelIndex * 4 + 0] = 255;	// R
				ptr[pixelIndex * 4 + 1] = 255;	// G
				ptr[pixelIndex * 4 + 2] = 0;	// B
				ptr[pixelIndex * 4 + 3] = 255;	// A
				*/
			}
		//	i++;
		}
	}

	SDL_SaveBMP(surface, "test.bmp");



	return surface;
}





int main(int argc, char *argv[])
{
	SDL_Surface* screen;
	utl::initSDL(SCREEN_WIDTH, SCREEN_HEIGHT, screen);

	SDL_Surface* test = createTestImage();



	/*
	string path = "testImages/";
	string image0Name = "lena.jpg";
	SDL_Surface* image0 = utl::loadSDLImage(path + image0Name);
	string image0DXT = path + "compressed " + image0Name;

	// assuming 8:1 compression ratio
	int numBytes = image0->w * image0->h * 4;
	int numCompressedBytes = numBytes / 8;
	uint8* compressedImage0Pixels = new uint8[numCompressedBytes];
	int outputBytes = 0;

	DXTConverter dxtConverter;
	dxtConverter.compressImageDXT1((uint8*)image0->pixels, (uint8*)compressedImage0Pixels, image0->w, image0->h, outputBytes);

	uint8* newImage0Pixels = new uint8[numBytes];
	dxtConverter.decompress(compressedImage0Pixels, newImage0Pixels, image0->w, image0->h);


	//Apply image to screen
	SDL_BlitSurface(image0, NULL, screen, NULL);
	*/
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









/*
void RotateAndSaveImg(string filename) {
	const int sprite_size = 200;

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
*/



