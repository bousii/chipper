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

void inc_pc(chip8_t *chip) { chip->pc += 2; }

void cycle(chip8_t *chip) {
	uint16_t opcode;
	uint8_t first;
	uint8_t x;
	uint8_t y;
	uint8_t kk;
	while (1) {
		opcode = chip->ram[chip->pc] << 8 | chip->ram[chip->pc + 1]; // get current instruction
		first = opcode >> 12; 
		x = opcode >> 8 & 0xF;
		y = opcode >> 4 & 0xF;
		kk = opcode & 0x00FF;
		switch (first) { // consider moving opcode processing to separate function
			case 0x0:
				switch (opcode) {
					case 0x00E0:
					//cls();
						break;
					case 0x00EE:
					//RET
						break;
					default:
					//SYS addr, ignore
						break;
				}
				break;
			case 0x1:
				chip->pc = opcode & 0x0FFF;
				break;
			case 0x2:
				chip->sp += 1;
				chip->stack[chip->sp] = chip->pc;
				break;
			case 0x3:
				if (chip->V[x] == kk) {
					inc_pc(chip);
				}
				break;
			case 0x4:
				if (chip->V[x] != kk) {
					inc_pc(chip);
				}
				break;
			case 0x5:
				if (chip->V[x] == chip->V[y]) {
					inc_pc(chip);
				}
				break;
			case 0x6:
				chip->V[x] = kk;
				break;
			case 0x7:
				chip->V[x] += kk;
				break;
			case 0x8:
				switch (opcode & 0xF) {
					case 0x0:
						chip->V[x] = chip->V[y];
						break;
					case 0x1:
						chip->V[x] |= chip->V[y];
						break;
					case 0x2:
						chip->V[x] &= chip->V[y];
						break;
					case 0x3:
						chip->V[x] ^= chip->V[y];
						break;
					case 0x4:
						chip->V[x] = (chip->V[x] + chip->V[y] & 0xFFFF);
						if (chip->V[x] + chip->V[y] > 0xFFFF) {
							chip->V[0xF] = 1;
						} else {
							chip->V[0xF] = 0;
						}
						break;
					case 0x5:
						chip->V[x] = (chip->V[x] - chip->V[y] & 0xFFFF);
						if (chip->V[x] > chip->V[y]) {
							chip->V[0xF] = 1;
						} else {
							chip->V[0xF] = 0;
						}
						break;
					case 0x6:
						chip->V[0xF] = chip->V[x] & 1;
						chip->V[x] >>= 1;
						break;
					case 0x7:
						chip->V[y] = (chip->V[y] - chip->V[x] & 0xFFFF);
						if (chip->V[y] > chip->V[x]) {
							chip->V[0xF] = 1;
						} else {
							chip->V[0xF] = 0;
						}
						break;
					case 0xE:
						chip->V[0xF] = chip->V[x] >> 15 & 1;
						chip->V[x] <<= 1;
						break;
				}
			case 0x9:
				if (chip->V[x] != chip->V[y]) {
					inc_pc(chip);
				}
				break;
			case 0xA:
				chip->I = opcode & 0xFFF;
				break;
			case 0xB:
				chip->pc = chip->V[0] + opcode & 0xFFF;
				break;
			case 0xC:
				chip->V[x] = (rand() % 256) & kk;
				break;
			//case 0xD:
				//do this one later, its pretty bad...
			//	break;
			case 0xE:
				switch (kk) {
					case 0x9E:
						//keyboard input
						break;
					case 0xA1:
						//keyboard input
						break;
				}
				break;
			case 0xF:
				switch (kk) {
					case 0x07:
						chip->V[x] = chip->delay;
						break;
					case 0x0A:
						break;
					case 0x15:
						chip->delay = chip->V[x];
						break;
					case 0x18:
						chip->sound = chip->V[x];
						break;
					case 0x1E:
						chip->I += chip->V[x];
						break;

				}
			default:
				printf("Unimplemented instruction\n");
				break;
		}

		inc_pc(chip);
	}
	

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
	
	cycle(chip);

cleanup:
	fclose(f);
file_clean:
	free(chip);
arg_clean:
	return exit;
}
