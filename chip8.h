#ifndef CHIP8_H
#define CHIP8_H
typedef struct {

/*  Program facing registers */
	uint8_t V[16];
	uint16_t I;
	uint8_t delay;
	uint8_t sound;
	
/* Non program facing pseudo-registers */
	uint16_t pc; // program counter
	uint8_t sp; // stack pointer
	
	uint16_t stack[16];
	uint8_t ram[4096]; // 4 kB of RAM
} chip8_t;
#endif
