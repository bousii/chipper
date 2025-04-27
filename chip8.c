#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include "assert.h"
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

void sleep_ms(int milliseconds)
{
	usleep(milliseconds * 1000);
}

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

	uint8_t *opcode_buffer = malloc(file_size * sizeof(uint8_t));
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

void inc_pc(chip8_t *chip) 
{
	chip->pc += 2;
	assert(chip->pc < RAM_SIZE);
}

int undefined_instruction(uint16_t opcode)
{
    printf("unimplemented opcode %X, aborting\n", opcode);
    return -1;

}

int handle_opcode(chip8_t *chip) {
	uint16_t opcode = chip->ram[chip->pc] << 8 | chip->ram[chip->pc + 1]; // get current instruction
	uint8_t first = opcode >> 12;
	uint8_t x = opcode >> 8 & 0xF;
	uint8_t y = opcode >> 4 & 0xF;
	uint8_t kk = opcode & 0x00FF;
	int ret = 0;
	if(opcode == 0) {
		printf("ZERO BABBOOEY");
	}
	printf("Opcode:%X kk:%X X:%X Y:%X First:%X\n", opcode, kk, x, y, first);
		switch (first) {
		case 0x0:
			switch (opcode) {
				case 0x00E0:
					for (size_t i = 0; i < VID_WIDTH; i++) {
						for(size_t j = 0; j < VID_HEIGHT; j++) {
							chip->video_buffer[i][j] = 0;
						}
					}
					break;
				case 0x00EE:
					chip->pc = chip->stack[chip->sp];
					chip->sp -= 1;
					break;
				default:
					ret = undefined_instruction(opcode);
					break;
			}
			break;
		case 0x1:
			chip->pc = opcode & 0x0FFF;
			chip->pc -= 2;
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
					chip->V[x] = ((chip->V[x] + chip->V[y]) & 0xFFFF);
					if (chip->V[x] + chip->V[y] > 0xFFFF) {
						chip->V[0xF] = 1;
					} else {
						chip->V[0xF] = 0;
					}
					break;
				case 0x5:
					if (chip->V[x] > chip->V[y]) {
						chip->V[0xF] = 1;
					} else {
						chip->V[0xF] = 0;
					}
					chip->V[x] = ((chip->V[x] - chip->V[y]) & 0xFFFF);
					break;
				case 0x6:
					chip->V[0xF] = chip->V[x] & 1;
					chip->V[x] >>= 1;
					break;
				case 0x7:
					if (chip->V[y] > chip->V[x]) {
						chip->V[0xF] = 1;
					} else {
						chip->V[0xF] = 0;
					}
					chip->V[y] = ((chip->V[y] - chip->V[x]) & 0xFFFF);
					break;
				case 0xE:
					chip->V[0xF] = chip->V[x] >> 15 & 1;
					chip->V[x] <<= 1;
					break;
				default:
					ret = undefined_instruction(opcode);
                    break;
			}
		case 0x9:
			if (chip->V[x] != chip->V[y]) {
				inc_pc(chip);
			}
			break;
		case 0xA:
			chip->I = opcode & 0xFFFu;
			break;
		case 0xB:
			chip->pc = chip->V[0] + (opcode & 0xFFF);
			break;
		case 0xC:
			chip->V[x] = (rand() % 256) & kk;
			break;
		case 0xD:
			for (uint8_t i = 0; i < (opcode & 0xF); i++) {
				unsigned char pixel = chip->ram[chip->I + i];
				for (uint8_t j = 0; j < 8; j++) {
					if ((pixel & (0x80 >> j)) != 0) {
						chip->V[0xF] |= chip->video_buffer[chip->V[x] + j][chip->V[y] + i];
						chip->video_buffer[chip->V[x] + j][chip->V[y] + i] ^= 1;
					}
				}
			}
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
				case 0x29:
					chip->I = fontset[chip->V[x] * 5 % 0xF];
					break;
				case 0x33:
					chip->ram[chip->I] = chip->V[x] / 100 % 10;
					chip->ram[chip->I+1] = chip->V[x] / 10 % 10;
					chip->ram[chip->I+2] = chip->V[x] % 10;
					break;
				case 0x55:
					for (int i = 0; i <= x; i++) {
						chip->ram[chip->I + i] = chip->V[i];
					}
					break;
				case 0x65:
					for (int i = 0; i <= x; i++) {
						chip->V[i] = chip->ram[chip->I + i];
					}
					break;
				default:
					ret = undefined_instruction(opcode);
					break;
			}
            break;
	    default:
		    ret = undefined_instruction(opcode);
            break;
	}
    return ret;
}

int cycle(chip8_t *chip) {

    if (handle_opcode(chip) < 0)
    {
        return -1;
    }
	inc_pc(chip);
	/* Add timer functionality here */
	sleep_ms(250);
	return 0;
}
