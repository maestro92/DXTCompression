#pragma once



#include <cstdlib>
#include <iostream>
#include <stdio.h>

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

#define C565_R_MASK		0xF800	// 1111 1000 0000 0000	minus the last three bits
#define C565_G_MASK		0x07E0	// 0000 0111 1110 0000	minus the last two bits
#define C565_B_MASK		0x001F	// 0000 0000 0001 1111	minus the last two bits


#define SCREEN_WIDTH	800
#define SCREEN_HEIGHT	600