

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
	image[pixelStart + 3] = 255;

}

void setImageAlpha(uint8* image, int pixelStart)
{
	image[pixelStart + 3] = 255;
}


void createImage(uint8* pixels, int width, int height)
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

			// uint32* ptr = (uint32*)surface->pixels;
			// uint32 color = 0xff00ffff;
			// ptr[pixelIndex] = color;
			// these two are equivalent
			/*
			if (x == 108 && y == 32)
			{
				cout << "ps " << ps << endl;
				cout << x << " " << y << endl;
			}
			*/

		//	uint32* ptr = (uint32*)image->pixels;
		//	ptr[ps] = pixels[ps];

			uint8* ptr = (uint8*)image->pixels;
		//	cout << x << " " << y << " " << ps << ", ";
		//	printPixel(ptr, ps);

		//	ptr[ps] = pixels[ps];

			setImageColor(ptr, ps, &pixels[ps]);

		//	cout << "ps " << ps << endl;
	//		cout << x << " " << y << endl;

		//	setImageAlpha((uint8*)(image->pixels), ps);
			// making sure the alpha is full
		}
	}

	SDL_SaveBMP(image, "new.bmp");
}







void printImage(uint8* image, int w, int h )
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
		//	int index = (y * w + x) * 4;
		//	uint8* ptr = image;

		//	cout << (int)(ptr[index + 0]) << " " << (int)(ptr[index + 1]) << " " << (int)(ptr[index + 2]) << " " << (int)(ptr[index + 3]) << ",	";

			if (x >= 8)
			{
				break;
			}
		}

		cout << endl;
	}
}



int main(int argc, char *argv[])
{
	SDL_Surface* screen;
	utl::initSDL(SCREEN_WIDTH, SCREEN_HEIGHT, screen);
	
	SDL_Surface* image2 = createTestImage();

	string path = "testImages/";
	string image0Name = "lena.jpg";
	SDL_Surface* image0 = utl::loadSDLImage(path + image0Name);
//	string image0DXT = path + "compressed " + image0Name;
	cout << "Printing Original Image" << endl;
	printImage((uint8*)image0->pixels, image0->w, image0->h);

	
	// assuming 8:1 compression ratio
	int numBytes = image0->w * image0->h * 4;
	int numCompressedBytes = numBytes / 8;
	cout << "numCompressedBytes " << numCompressedBytes << endl;
	uint8* compressedImage0Pixels = new uint8[numCompressedBytes];
	int outputBytes = 0;

	DXTConverter dxtConverter;
	dxtConverter.compressImageDXT1((uint8*)image0->pixels, (uint8*)compressedImage0Pixels, image0->w, image0->h, outputBytes);

	uint8* newImage0Pixels = new uint8[numBytes];
	dxtConverter.decompress(compressedImage0Pixels, newImage0Pixels, image0->w, image0->h);
	
//	cout << "Printing new Image" << endl;
//	printImage(newImage0Pixels, image0->w, image0->h);

	createImage(newImage0Pixels, image0->w, image0->h);


	cout << "Done Creating Image" << endl;

	while (1)
	{
	}
//	

	//Apply image to screen
	SDL_BlitSurface(image0, NULL, screen, NULL);
	
	//Update Screen
	SDL_Flip(screen);

	//Pause
	SDL_Delay(2000);

	

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



