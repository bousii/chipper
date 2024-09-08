#define START_ADDRESS 0x200
#define RAM_OFFSET 0x200
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define FONTSET_OFFSET 0x50
#define FONTSET_SIZE 80
#define KEY_COUNT 16
#define VID_WIDTH 64
#define VID_HEIGHT 32

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
	

/* Graphics backend */
	uint8_t keys[KEY_COUNT];
	uint8_t video_buffer[VID_WIDTH][VID_HEIGHT];

} chip8_t;

void load_fonts(chip8_t *chip);
int load_rom(chip8_t *chip, FILE *f);
void inc_pc(chip8_t *chip);
void handle_opcode(chip8_t *chip, uint16_t opcode);
void cycle(chip8_t *chip);

#endif
