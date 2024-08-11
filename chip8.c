#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "chip8.h"

#define RAM_OFFSET 0x200




//load_rom into ram


int main(int argc, char *argv[]) {
	FILE *f;
	chip8_t *chip;
	uint16_t opcode_buffer;
	if (argc != 2) {
		printf("Please input the path to the ROM file, exiting...\n");
		exit(1);
	}

	f = fopen(argv[1], "rb");
	if (f == NULL) {
		printf("Invalid path, exiting...\n");
		exit(1);
	}

	fclose(f);
	return 0;
}
