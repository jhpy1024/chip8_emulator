#include "chip8.h"

#include <stdio.h>
#include <stdlib.h>

static void (*chip8_ops[16])(chip8* c) =
{
    chip8_op_0, chip8_op_1, chip8_op_2, chip8_op_3,
    chip8_op_4, chip8_op_5, chip8_op_6, chip8_op_7,
    chip8_op_8, chip8_op_9, chip8_op_a, chip8_op_b,
    chip8_op_c, chip8_op_d, chip8_op_e, chip8_op_f
};

chip8* chip8_new(char* filename)
{
    chip8* c = malloc(sizeof(struct chip8));
    chip8_init(c, filename);
    return c;
}

void chip8_free(chip8* c)
{
    free(c);
}

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
    chip8_execute_opcode(c);
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

    for (int i = 0, chr = 1; (chr = fgetc(fp)) != EOF; ++i)
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

void chip8_execute_opcode(chip8* c)
{
    (chip8_ops[(c->current_opcode & 0xF000) >> 12])(c);
}

void chip8_op_0(chip8* c)
{

}

void chip8_op_1(chip8* c)
{

}

void chip8_op_2(chip8* c)
{

}

void chip8_op_3(chip8* c)
{

}

void chip8_op_4(chip8* c)
{

}

void chip8_op_5(chip8* c)
{

}

void chip8_op_6(chip8* c)
{

}

void chip8_op_7(chip8* c)
{

}

void chip8_op_8(chip8* c)
{

}

void chip8_op_9(chip8* c)
{

}

void chip8_op_a(chip8* c)
{

}

void chip8_op_b(chip8* c)
{

}

void chip8_op_c(chip8* c)
{

}

void chip8_op_d(chip8* c)
{

}

void chip8_op_e(chip8* c)
{

}

void chip8_op_f(chip8* c)
{

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

/*
void chip8_op_set_address(chip8* c)
{
    c->address_register = c->current_opcode & 0x0FFF;
    c->program_counter += 2;
}
*/

/*
void chip8_op_clear_screen(chip8* c)
{
    chip8_clear_graphics(c);
}
*/

/*
void chip8_op_call_sub(chip8* c)
{
    c->stack[c->stack_pointer++] = c->program_counter;
    c->program_counter = c->current_opcode & 0x0FFF;
}
*/

/*
void chip8_op_jump(chip8* c)
{
    c->program_counter = c->current_opcode & 0x0FFF;
}
*/

/*
void chip8_op_add_y_to_x(chip8* c)
{
    int x_index = (c->current_opcode & 0x0F00) >> 8;
    int y_index = (c->current_opcode & 0x00F0) >> 4;
    unsigned char x = c->registers[x_index];
    unsigned char y = c->registers[y_index];

    c->registers[0xF] = y > (0xFF - x) ? 1 : 0;
    c->registers[x_index] += c->registers[y_index];
    c->program_counter += 2;
}
*/
