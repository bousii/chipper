#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include "chip8.h"



uint8_t fontset[FONTSET_SIZE] =
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};


void load_fonts(chip8_t *chip) {
	for(size_t i = 0; i < FONTSET_SIZE; i++) {
		chip->ram[FONTSET_OFFSET + i] = fontset[i];
	}
}

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
		chip->ram[START_ADDRESS + i] = opcode_buffer[i];
	}

	exit:
	free(opcode_buffer);
	return ret;
}

void inc_pc(chip8_t *chip) { chip->pc += 2; }

void handle_opcode(chip8_t *chip, uint16_t opcode) {
	uint8_t first;
	uint8_t x;
	uint8_t y;
	uint8_t kk;
	first = opcode >> 12; 
	x = opcode >> 8 & 0xF;
	y = opcode >> 4 & 0xF;
	kk = opcode & 0x00FF;
	if(opcode == 0) {
		printf("ZERO BABBOOEY");
	}
	printf("Opcode:%X kk:%X X:%X Y:%X First:%X\n", opcode, kk, x, y, first);
		switch (first) {
		case 0x0:
			switch (opcode) {
				case 0x00E0:
					//memset(chip->video_buffer, 0, sizeof(chip->video_buffer));
					break;
				case 0x00EE:
					chip->pc = chip->stack[chip->sp];
					chip->sp -= 1;
					break;
			}
			break;
		case 0x1:
			chip->pc = opcode & 0x0FFF;
			break;
		case 0x2:
			chip->stack[chip->sp] = chip->pc;
			chip->sp += 1;
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
					chip->V[x] = ((chip->V[x] + chip->V[y]) & 0xFFFF);
					if (chip->V[x] + chip->V[y] > 0xFFFF) {
						chip->V[0xF] = 1;
					} else {
						chip->V[0xF] = 0;
					}
					break;
				case 0x5:
					chip->V[x] = ((chip->V[x] - chip->V[y]) & 0xFFFF);
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
					chip->V[y] = ((chip->V[y] - chip->V[x]) & 0xFFFF);
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
			chip->pc = (chip->V[0] + opcode) & 0xFFF;
			break;
		case 0xC:
			chip->V[x] = (rand() % 256) & kk;
			break;
		case 0xD:
			printf("Graphics stuff\n");
			break;
		case 0xE:
			switch (kk) {
				case 0x9E:
					printf("Input stuff\n");
					break;
				case 0xA1:
					printf("Input stuff\n");
					break;
			}
			break;
		case 0xF:
			switch (kk) {
				case 0x07:
					chip->V[x] = chip->delay;
					break;
				case 0x0A:
					printf("More input stuff\n");
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
			printf("%X unimp\n", opcode);
			break;
	}
}

void cycle(chip8_t *chip) {
	uint16_t opcode = chip->ram[chip->pc] << 8 | chip->ram[chip->pc + 1]; // get current instruction
	handle_opcode(chip, opcode);
	inc_pc(chip);
	/* Add timer functionality here */
	sleep(1);
}
