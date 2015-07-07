#ifndef CHIP8_H
#define CHIP8_H

#define NUM_REGISTERS 16
#define MEMORY_SIZE 4096
#define WIDTH 64
#define HEIGHT 32
#define STACK_SIZE 16
#define FONTSET_SIZE 80
#define NUM_KEYS 16
#define START_OFFSET 0x200

unsigned char chip8_fontset[FONTSET_SIZE];

typedef struct chip8
{
    unsigned char memory[MEMORY_SIZE];
    unsigned char registers[NUM_REGISTERS];
    unsigned char graphics[WIDTH * HEIGHT];

    unsigned short current_opcode;
    unsigned short address_register;
    unsigned short program_counter;

    unsigned char delay_timer;
    unsigned char sound_timer;

    unsigned char stack[STACK_SIZE];
    unsigned short stack_pointer;

    unsigned char keys[NUM_KEYS];
} chip8;

void chip8_init(chip8* c);
void chip8_emulate_cycle(chip8* c);

void chip8_load_program(chip8* c, char* filename);
void chip8_clear_graphics(chip8* c);
void chip8_clear_stack(chip8* c);
void chip8_clear_registers(chip8* c);
void chip8_clear_memory(chip8* c);
void chip8_reset_timers(chip8* c);
void chip8_load_fontset(chip8* c);
void chip8_fetch_opcode(chip8* c);
void chip8_update_timers(chip8* c);

#endif
