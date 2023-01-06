#pragma once

#define RASTER_WIDTH 720
#define RASTER_HEIGHT 500

#define STARS_COUNT 3000

const unsigned int TOTAL_PIXELS = (RASTER_WIDTH * RASTER_HEIGHT);

#define GRID_COUNT 11

const unsigned int GRID_SIZE = GRID_COUNT * 2;
const float FRAME_RATE = (1.0f / 60.0f);

#define PI 3.14159265358979323846 

const unsigned int RED_CHANNEL = 0x00ff0000;
const unsigned int BLUE_CHANNEL = 0x000000ff;
const unsigned int GREEN_CHANNEL = 0x0000ff00;
const unsigned int ALPHA_CHANNEL = 0xff000000;

const unsigned int RED = (RED_CHANNEL | ALPHA_CHANNEL);
const unsigned int BLUE = (BLUE_CHANNEL | ALPHA_CHANNEL);
const unsigned int GREEN = (GREEN_CHANNEL | ALPHA_CHANNEL);

const unsigned int WHITE = (RED | GREEN | BLUE);

const unsigned int YELLOW = (RED_CHANNEL | GREEN_CHANNEL);
const unsigned int CYAN = (GREEN_CHANNEL | BLUE_CHANNEL);
const unsigned int MAGENTA = (RED_CHANNEL | BLUE_CHANNEL);

#define SHOW_TRIANGLE_OUTLINES 0
//#define SHOW_TRIANGLE_VERTEX_NORMALS 0
