#include "peripherals.h"
#include "chip8.h"
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>


void update(uint8_t *video_buffer, Peripheral *perif) {
	for (int i = 0; i < VID_WIDTH; i++) {
		for (int j = 0; j < VID_HEIGHT; j++) {
			
		}
	}
}

int initialize(Peripheral *perif) {

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Failed to initialize SDL2 Library\n");
		return -1;
	}

	perif->window = SDL_CreateWindow("Chipper", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);

	if (!(perif->window)) {
		printf("Failed to create window\n");
		return -1;
	}

	perif->window_surface = SDL_GetWindowSurface(perif->window);

	if (!(perif->window_surface)) {
		printf("Failed to get the surface from the window\n");
		return -1;
	}

	for (int i = 0; i < VID_WIDTH; i++) {
		for (int j = 0; j < VID_HEIGHT; j++) {
			perif->pixels[i][j].x = i * 10;
			perif->pixels[i][j].y = j * 10;
			perif->pixels[i][j].w = PIXEL_WIDTH;
			perif->pixels[i][j].h = PIXEL_HEIGHT;
		}
	}
	perif->gRenderer = SDL_CreateRenderer(perif->window, -1, SDL_RENDERER_ACCELERATED);
	return 0;
}
