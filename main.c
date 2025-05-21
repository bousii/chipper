#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include "chip8.h"
#include "peripherals.h"

int main(int argc, char *argv[]) {
	FILE *f;
	chip8_t *chip;
	Peripheral *perif;
	int exit = 0;

	if (argc != 2) {
		printf("Please input the path to the ROM file\n");
		exit = -1;
		goto arg_clean;
	}
	chip = calloc(1, sizeof(*chip));
	chip->pc = START_ADDRESS;

	f = fopen(argv[1], "rb");
	if (f == NULL) {
		printf("Invalid path, exiting...\n");
		exit = -1;
		goto file_clean;
	}
	int ret = load_rom(chip, f);
	if (ret != 0) {
		exit = -1;
		goto cleanup;
	}

	perif = calloc(1, sizeof(*perif));
	int val = initialize(perif);
	if (val != 0) {
		goto cleanup;
	}
	load_fonts(chip);
	SDL_Event e;
	uint32_t last_timer_tick = 0;
	bool quit = false; 
	while(!quit) {

		while( SDL_PollEvent( &e ) ) {
			switch (e.type) {
				case SDL_KEYDOWN:
					handle_input(chip, e.key.keysym.sym, 1);
					break;
				case SDL_KEYUP:
					handle_input(chip, e.key.keysym.sym, 0);
					break;
				case SDL_QUIT:
					quit = true;
					break;
			}
		}

		if (SDL_GetTicks() - last_timer_tick >= 1000/60) {//60 Hz
			uint8_t *d = &chip->delay;
			uint8_t *s = &chip->sound;
			if (*d > 0) (*d)--;
			if (*s > 0) (*s)--;
			last_timer_tick = SDL_GetTicks();
		}
		update(chip->video_buffer, perif);

		if (cycle(chip) < 0)
		{
			quit = true;
			break;
		}
	}

cleanup:
	SDL_DestroyRenderer(perif->gRenderer);
	SDL_DestroyWindow(perif->window);
	SDL_Quit();
	fclose(f);
file_clean:
	free(perif);
	free(chip);
arg_clean:
	return exit;
}
