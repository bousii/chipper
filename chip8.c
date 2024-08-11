#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "chip8.h"

#define RAM_OFFSET 0x200

//load_rom into ram
int load_rom(chip8_t *chip, FILE *f) {
	fseek(f, 0, SEEK_END);
	long file_size = ftell(f);
	fseek(f,0,SEEK_SET);

	uint8_t *opcode_buffer = (uint8_t*)malloc(file_size * sizeof(uint8_t));
	size_t bytes_read = fread(opcode_buffer, sizeof(uint8_t), file_size, f);
	int ret = 0;
	if(bytes_read != file_size) {
		ret = 1;
		goto exit;
	}

	for (size_t i = 0; i < bytes_read; i++) {
		chip->ram[RAM_OFFSET + i] = opcode_buffer[i];
		printf("%.2X ", chip->ram[RAM_OFFSET + i]);
		if (i % 2 != 0) {
			printf("\n");
		}
	}
exit:
	free(opcode_buffer);
	return ret;
}


int main(int argc, char *argv[]) {
	FILE *f;
	chip8_t *chip;
	int exit = 0;
	if (argc != 2) {
		printf("Please input the path to the ROM file, exiting...\n");
		exit = 1;
		goto arg_clean;
	}
	chip = calloc(0, sizeof(*chip));
	

	f = fopen(argv[1], "rb");
	if (f == NULL) {
		printf("Invalid path, exiting...\n");
		exit = 1;
		goto file_clean;
	}
	int ret = load_rom(chip, f);
	if (ret != 0) {
		exit = 1;
		goto cleanup;
	}


cleanup:
	fclose(f);
file_clean:
	free(chip);
arg_clean:
	return exit;
}
