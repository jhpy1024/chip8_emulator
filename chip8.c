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

/* 
 * 0NNN: Calls RCA 1802 program at address NNN.
 * 00E0: Clears the screen.
 * 00EE: Returns from a subroutine.
 */
void chip8_op_0(chip8* c)
{
    c->program_counter += 2;
}

/*
 * 1NNN: Jumps to address NNN.
 */
void chip8_op_1(chip8* c)
{
    c->program_counter += 2;
}

/*
 * 2NNN: Calls subroutine at NNN.
 */
void chip8_op_2(chip8* c)
{
    c->program_counter += 2;
}

/*
 * 3XNN: Skips the next instruction if VX equals NN.
 */
void chip8_op_3(chip8* c)
{
    int reg = (c->current_opcode & 0x0F00) >> 8;
    char nn = c->current_opcode & 0x00FF;

    if (c->registers[reg] == nn)
    {
        c->program_counter += 2;
    }

    c->program_counter += 2;
}

/*
 * 4XNN: Skips the next instruction if VX doesn't equal NN.
 */
void chip8_op_4(chip8* c)
{
    int reg = (c->current_opcode & 0x0F00) >> 8;
    char nn = c->current_opcode & 0x00FF;

    if (c->registers[reg] != nn)
    {
        c->program_counter += 2;
    }

    c->program_counter += 2;
}

/*
 * 5XY0: Skips the next instruction if VX equals VY.
 */
void chip8_op_5(chip8* c)
{
    char vx = c->registers[(c->current_opcode & 0x0F00) >> 8];
    char vy = c->registers[(c->current_opcode & 0x00F0) >> 4];

    if (vx == vy)
    {
        c->program_counter += 2;
    }

    c->program_counter += 2;
}

/*
 * 6XNN: Sets VX to NN.
 */
void chip8_op_6(chip8* c)
{
    c->registers[(c->current_opcode & 0x0F00) >> 8] = c->current_opcode & 0x00FF;
    c->program_counter += 2;
}

/*
 * 7XNN: Adds NN to VX.
 */
void chip8_op_7(chip8* c)
{
    c->registers[(c->current_opcode & 0x0F00) >> 8] += c->current_opcode & 0x00FF;
    c->program_counter += 2;
}

/*
 * 8XY0: Sets VX to the value of VY.
 * 8XY1: Sets VX to VX or VY.
 * 8XY2: Sets VX to VX and VY.
 * 8XY3: Sets VX to VX xor VY.
 * 8XY4: Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
 * 8XY5: VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
 * 8XY6: Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift.
 * 8XY7: Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
 * 8XYE: Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift.
 */
void chip8_op_8(chip8* c)
{
    int x = (c->current_opcode & 0x0F00) >> 8;
    int y = (c->current_opcode & 0x00F0) >> 4;

    switch (c->current_opcode & 0x000F)
    {
        case 0x0000:
            c->registers[x] = c->registers[y];
            break;
        case 0x0001:
            c->registers[x] = c->registers[x] | c->registers[y];
            break;
        case 0x0002:
            c->registers[x] = c->registers[x] & c->registers[y];
            break;
        case 0x0003:
            c->registers[x] = c->registers[x] ^ c->registers[y];
            break;
        case 0x0004:
            break;
        case 0x0005:
            break;
        case 0x0006:
            c->registers[0xF] = c->registers[x] & 0xF;
            c->registers[x] = c->registers[x] >> 1;
            break;
        case 0x0007:
            break;
        case 0x000E:
            c->registers[0xF] = c->registers[x] & 0xF;
            c->registers[x] = c->registers[x] << 1;
            break;
        default:
            break;
    }

    c->program_counter += 2;
}

/*
 * 9XY0: Skips the next instruction if VX doesn't equal VY.
 */
void chip8_op_9(chip8* c)
{
    c->program_counter += 2;
}

/*
 * ANNN: Sets I to the address NNN.
 */
void chip8_op_a(chip8* c)
{
    c->program_counter += 2;
}

/*
 * BNNN: Jumps to the address NNN plus V0.
 */
void chip8_op_b(chip8* c)
{
    c->program_counter += 2;
}

/*
 * CXNN: Sets VX to a random number, masked by NN.
 */
void chip8_op_c(chip8* c)
{
    c->program_counter += 2;
}

/*
 * DXYN: Sprites stored in memory at location in index register (I), maximum 8bits wide.
 *       Wraps around the screen. If when drawn, clears a pixel, register VF is set to 1 otherwise it is zero.
 *       All drawing is XOR drawing (i.e. it toggles the screen pixels)
 */
void chip8_op_d(chip8* c)
{
    c->program_counter += 2;
}

/*
 * EX9E: Skips the next instruction if the key stored in VX is pressed.
 * EXA1: Skips the next instruction if the key stored in VX isn't pressed.
 */
void chip8_op_e(chip8* c)
{
    c->program_counter += 2;
}

/*
 * FX07: Sets VX to the value of the delay timer.
 * FX0A: A key press is awaited, and then stored in VX.
 * FX15: Sets the delay timer to VX.
 * FX18: Sets the sound timer to VX.
 * FX1E: Adds VX to I.
 * FX29: Sets I to the location of the sprite for the character in VX.
 *       Characters 0-F (in hexadecimal) are represented by a 4x5 font.
 * FX33: Stores the Binary-coded decimal representation of VX, with the most significant
 *       of three digits at the address in I, the middle digit at I plus 1, and the least significant
 *       digit at I plus 2. (In other words, take the decimal representation of VX, place the hundreds
 *       digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.)
 * FX55: Stores V0 to VX in memory starting at address I.
 * FX65: Fills V0 to VX with values from memory starting at address I.
 */
void chip8_op_f(chip8* c)
{
    int x = (c->current_opcode & 0x0F00) >> 8;

    switch (c->current_opcode & 0x000F)
    {
        case 0x0007:
            c->registers[x] = c->delay_timer;
            break;
        case 0x000A:
            break;
        case 0x0005:
            break;
        case 0x0008:
            c->sound_timer = c->registers[x];
            break;
        case 0x000E:
            c->address_register += c->registers[x];
            break;
        case 0x0009:
            break;
        case 0x0003:
            break;
        default:
            break;
    }

    c->program_counter += 2;
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
