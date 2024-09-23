#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>

#define VID_WIDTH 64
#define VID_HEIGHT 32
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 320
#define PIXEL_HEIGHT 10
#define PIXEL_WIDTH 10
#ifndef PERIPHERALS_H
#define PERIPHERALS_H
typedef struct {
	SDL_Renderer *gRenderer;
	SDL_Window *window;
	SDL_Surface *window_surface;
	SDL_Rect pixels[VID_WIDTH][VID_HEIGHT];

	//add keyboard input
} Peripheral;

void update(uint8_t video_buffer[VID_WIDTH][VID_HEIGHT], Peripheral *perif);
int initialize(Peripheral *perif);
#endif
