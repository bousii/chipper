#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>

#ifndef PERIPHERALS_H
#define PERIPHERALS_H
typedef struct {
	SDL_Renderer *gRenderer;
	SDL_Window *window;
	SDL_Surface *window_surface;

	//add keyboard input
} Peripheral;

#endif
