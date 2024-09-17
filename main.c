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
		printf("Please input the path to the ROM file, exiting...\n");
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

	SDL_Event e;
	bool quit = false; 
	SDL_Rect fillRect = { SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
	while(!quit) { 
        SDL_SetRenderDrawColor( perif->gRenderer, 0xFF, 0x00, 0x00, 0xFF );
        SDL_RenderFillRect( perif->gRenderer, &fillRect );
		SDL_RenderPresent(perif->gRenderer);
		cycle(chip);
		while( SDL_PollEvent( &e ) ) {
			if( e.type == SDL_QUIT )
				quit = true;
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
