#include "chip8.h"

#include <stdio.h>

void chip8_init(chip8* c, char* filename)
{
    c->program_counter = 0x200;
    c->current_opcode = 0;
    c->address_register = 0;
    c->stack_pointer = 0;

    chip8_clear_graphics(c);
    chip8_clear_stack(c);
    chip8_clear_registers(c);
    chip8_clear_memory(c);
    chip8_load_fontset(c);
    chip8_load_program(c, filename);
}

void chip8_emulate_cycle(chip8* c)
{
    chip8_fetch_opcode(c);
    chip8_update_timers(c);
}

void chip8_load_program(chip8* c, char* filename)
{
    FILE* fp = fopen(filename, "rb");
    if (!fp)
    {
        fprintf(stderr, "Unable to load program with filename %s\n", filename);
        fclose(fp);
        exit(1);
    }

    for (int i = 0, char chr = 0; (chr = fgetc(fp)) != EOF; ++i)
    {
        c->memory[i + START_OFFSET] = chr;
    }

    fclose(fp);
}

void chip8_clear_graphics(chip8* c)
{
    for (int i = 0; i < WIDTH * HEIGHT; ++i)
    {
        c->graphics[i] = 0;
    }
}

void chip8_clear_stack(chip8* c)
{
    for (int i = 0; i < STACK_SIZE; ++i)
    {
        c->stack[i] = 0;
    }

    c->stack_pointer = 0;
}

void chip8_clear_registers(chip8* c)
{
    for (int i = 0; i < NUM_REGISTERS; ++i)
    {
        c->registers[i] = 0;
    }
}

void chip8_clear_memory(chip8* c)
{
    for (int i = 0; i < MEMORY_SIZE; ++i)
    {
        c->memory[i] = 0;
    }
}

void chip8_reset_timers(chip8* c)
{
    c->delay_timer = 0;
    c->sound_timer = 0;
}

void chip8_load_fontset(chip8* c)
{
    for (int i = 0; i < 80; ++i)
    {
        c->memory[i] = chip8_fontset[i];
    }
}

void chip8_fetch_opcode(chip8* c)
{
    /* Fetch two bytes from memory, combine to form the opcode */
    c->current_opcode = c->memory[c->program_counter] << 8 | c->memory[c->program_counter + 1];
}

void chip8_update_timers(chip8* c)
{
    if (c->delay_timer > 0)
    {
        c->delay_timer--;
    }

    if (c->sound_timer > 0)
    {
        if (c->sound_timer == 1)
        {
            puts("BEEP");
        }

        c->sound_timer--;
    }
}
