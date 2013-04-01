#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "nes.h"
#include "cpu.h"

#define ARG8            (cpu)->mem[(cpu)->regs.pc + 1]
#define ARG16           (((uint16_t)((cpu)->mem[(cpu)->regs.pc + 2])) << 8 | ARG8)

#define LOAD8(n)        (cpu)->mem[(n)]
#define LOAD16(n)       (LOAD8(n + 1) << 8 | LOAD8(n))

#define STORE8(n, v)    (cpu)->mem[(n)] = (v)

void
_call_brk (struct cpu * cpu, uint8_t op)
{
    /*
      BRK causes a non-maskable interrupt and increments the program
      counter by one. Therefore an RTI will go to the address of the
      BRK +2 so that BRK may be used to replace a two-byte instruction
      for debugging and the subsequent RTI will be correct.
     */
    cpu->regs.new_pc += 2;
}

void
_call_ora (struct cpu * cpu, uint8_t op)
{
    switch (op) {
    case 0x09: // immediate
        cpu->regs.a |= ARG8;
        break ;
    case 0x05: // zero page
        cpu->regs.a |= LOAD8(ARG8);
        break ;
    case 0x15: // zero page, x
        cpu->regs.a |= LOAD8(ARG8 + cpu->regs.x);
        break ;
    case 0x0D: // absolute
        cpu->regs.a |= LOAD8(ARG16);
        break ;
    case 0x1D: // absolute, x
        cpu->regs.a |= LOAD8(ARG16 + cpu->regs.x);
        break ;
    case 0x19: // absolute, y
        cpu->regs.a |= LOAD8(ARG16 + cpu->regs.y);
        break ;
    case 0x01: // indirect, x
        cpu->regs.a |= LOAD8(LOAD16(ARG8 + cpu->regs.x));
        break ;
    case 0x11: // indirect, y
        cpu->regs.a |= LOAD8(LOAD16(ARG8) + cpu->regs.y);
        break ;
    }
    if (cpu->regs.a == 0)
        cpu->regs.z = 1;
    if (cpu->regs.a & 0x80)
        cpu->regs.n = 1;
}

void
_call_asl (struct cpu *cpu, uint8_t op)
{
    switch (op) {
    case 0x0A: // accumulator
        cpu->regs.c = cpu->regs.a & (0x80);
        cpu->regs.a <<= 1;
        break ;
    case 0x06: // zero page
        cpu->regs.c = LOAD8(ARG8) & (0x80);
        LOAD8(ARG8) <<= 1;
        break ;
    case 0x16: // zero page, x
        cpu->regs.c = LOAD8(ARG8 + cpu->regs.x) & (0x80);
        LOAD8(ARG8 + cpu->regs.x) <<= 1;
        break ;
    case 0x0E: // absolute
        cpu->regs.c = LOAD8(ARG16) & (0x80);
        LOAD8(ARG16) <<= 1;
        break ;
    case 0x1E: // absolute, x
        cpu->regs.c = LOAD8(ARG16 + cpu->regs.x) & (0x80);
        LOAD8(ARG16 + cpu->regs.x) <<= 1;
        break ;
    }
}

void
_call_php (struct cpu * cpu, uint8_t op)
{
    cpu->regs.s--;
    cpu->mem[0x100 + cpu->regs.s] = cpu->regs.p;
}

void
_call_bpl (struct cpu * cpu, uint8_t op)
{
    if (cpu->regs.n == 0)
        cpu->regs.new_pc += (int8_t)ARG8;
}

void
_call_clc (struct cpu * cpu, uint8_t op)
{
    cpu->regs.c = 0;
}

void
_call_jsr (struct cpu * cpu, uint8_t op)
{
    cpu->mem[++cpu->regs.s] = cpu->regs.pc >> 8;
    cpu->mem[++cpu->regs.s] = cpu->regs.pc & 0x00FF;
    cpu->regs.pc = ARG16 - 1;
}

void
_call_and (struct cpu * cpu, uint8_t op)
{
    switch (op) {
    case 0x29: // immediate
        cpu->regs.a &= ARG8;
        break ;
    case 0x25: // zero page
        cpu->regs.a &= LOAD8(ARG8);
        break ;
    case 0x35: // zero page, x
        cpu->regs.a &= LOAD8(ARG8 + cpu->regs.x);
        break ;
    case 0x2D: // absolute
        cpu->regs.a &= LOAD8(ARG16);
        break ;
    case 0x3D: // absolute, x
        cpu->regs.a &= LOAD8(ARG16 + cpu->regs.x);
        break ;
    case 0x39: // absolute, y
        cpu->regs.a &= LOAD8(ARG16 + cpu->regs.y);
        break ;
    case 0x21: // indirect, x
        cpu->regs.a &= LOAD8(LOAD16(ARG8 + cpu->regs.x));
        break ;
    case 0x31: // indirect, y
        cpu->regs.a &= LOAD8(LOAD16(ARG8) + cpu->regs.y);
        break ;
    }
    if (cpu->regs.a == 0)
        cpu->regs.z = 1;
    if (cpu->regs.a & 0x80)
        cpu->regs.n = 1;
}

void
_call_bit (struct cpu * cpu, uint8_t op)
{
}

void
_call_rol (struct cpu * cpu, uint8_t op)
{
    switch (op) {
    case 0x2A: // accumulator
        cpu->regs.c = cpu->regs.a & (0x80);
        cpu->regs.a <<= 1;
        cpu->regs.a |= cpu->regs.c;
        break ;
    case 0x26: // zero page
        cpu->regs.c = LOAD8(ARG8) & (0x80);
        LOAD8(ARG8) <<= 1;
        LOAD8(ARG8) |= cpu->regs.c;
        break ;
    case 0x36: // zero page, x
        cpu->regs.c = LOAD8(ARG8 + cpu->regs.x) & (0x80);
        LOAD8(ARG8 + cpu->regs.x) <<= 1;
        LOAD8(ARG8 + cpu->regs.x) |= cpu->regs.c;
        break ;
    case 0x2E: // absolute
        cpu->regs.c = LOAD8(ARG16) & (0x80);
        LOAD8(ARG16) <<= 1;
        LOAD8(ARG16) |= cpu->regs.c;
        break ;
    case 0x3E: // absolute, x
        cpu->regs.c = LOAD8(ARG16 + cpu->regs.x) & (0x80);
        LOAD8(ARG16 + cpu->regs.x) <<= 1;
        LOAD8(ARG16 + cpu->regs.x) |= cpu->regs.c;
        break ;
    }
}


void
_call_plp (struct cpu * cpu, uint8_t op)
{
    cpu->regs.p = cpu->mem[0x100 + cpu->regs.s];
    cpu->regs.s++;
}


void
_call_bmi (struct cpu * cpu, uint8_t op)
{
    if (cpu->regs.n == 1)
        cpu->regs.new_pc += (int8_t)ARG8;
}


void
_call_sec (struct cpu * cpu, uint8_t op)
{
    cpu->regs.c = 1;
}


void
_call_rti (struct cpu * cpu, uint8_t op)
{
}


void
_call_eor (struct cpu * cpu, uint8_t op)
{
    switch (op) {
    case 0x49: // immediate
        cpu->regs.a ^= ARG8;
        break ;
    case 0x45: // zero page
        cpu->regs.a ^= LOAD8(ARG8);
        break ;
    case 0x55: // zero page, x
        cpu->regs.a ^= LOAD8(ARG8 + cpu->regs.x);
        break ;
    case 0x4D: // absolute
        cpu->regs.a ^= LOAD8(ARG16);
        break ;
    case 0x5D: // absolute, x
        cpu->regs.a ^= LOAD8(ARG16 + cpu->regs.x);
        break ;
    case 0x59: // absolute, y
        cpu->regs.a ^= LOAD8(ARG16 + cpu->regs.y);
        break ;
    case 0x41: // indirect, x
        cpu->regs.a ^= LOAD8(LOAD16(ARG8 + cpu->regs.x));
        break ;
    case 0x51: // indirect, y
        cpu->regs.a ^= LOAD8(LOAD16(ARG8) + cpu->regs.y);
        break ;
    }
    if (cpu->regs.a == 0)
        cpu->regs.z = 1;
    if (cpu->regs.a & 0x80)
        cpu->regs.n = 1;
}

void
_call_lsr (struct cpu * cpu, uint8_t op)
{
    switch (op) {
    case 0x4A: // accumulator
        cpu->regs.c = cpu->regs.a & (0x01);
        cpu->regs.a >>= 1;
        break ;
    case 0x46: // zero page
        cpu->regs.c = LOAD8(ARG8) & (0x01);
        LOAD8(ARG8) >>= 1;
        break ;
    case 0x56: // zero page, x
        cpu->regs.c = LOAD8(ARG8 + cpu->regs.x) & (0x01);
        LOAD8(ARG8 + cpu->regs.x) >>= 1;
        break ;
    case 0x4E: // absolute
        cpu->regs.c = LOAD8(ARG16) & (0x01);
        LOAD8(ARG16) >>= 1;
        break ;
    case 0x5E: // absolute, x
        cpu->regs.c = LOAD8(ARG16 + cpu->regs.x) & (0x01);
        LOAD8(ARG16 + cpu->regs.x) >>= 1;
        break ;
    }
}

void
_call_pha (struct cpu * cpu, uint8_t op)
{
    cpu->regs.s--;
    cpu->mem[0x100 + cpu->regs.s] = cpu->regs.a;
}

void
_call_jmp (struct cpu * cpu, uint8_t op)
{
    switch (op) {
    case 0x4C: // Absolute
        cpu->regs.new_pc = ARG16;
        break ;
    case 0x6C: // Indirect
        cpu->regs.new_pc = LOAD16(ARG16);
        break ;
    }
}

void
_call_bvc (struct cpu * cpu, uint8_t op)
{
    if (cpu->regs.v == 0)
        cpu->regs.new_pc += (int8_t)ARG8;
}

void
_call_cli (struct cpu * cpu, uint8_t op)
{
    cpu->regs.i = 0;
}

void
_call_rts (struct cpu * cpu, uint8_t op)
{
}

void
_call_adc (struct cpu * cpu, uint8_t op)
{
    switch (op) {
    case 0x69: // immediate
        cpu->regs.a += ARG8;
        break ;
    case 0x65: // zero page
        cpu->regs.a += LOAD8(ARG8);
        break ;
    case 0x75: // zero page, x
        cpu->regs.a += LOAD8(ARG8 + cpu->regs.x);
        break ;
    case 0x6D: // absolute
        cpu->regs.a += LOAD8(ARG16);
        break ;
    case 0x7D: // absolute, x
        cpu->regs.a += LOAD8(ARG16 + cpu->regs.x);
        break ;
    case 0x79: // absolute, y
        cpu->regs.a += LOAD8(ARG16 + cpu->regs.y);
        break ;
    case 0x61: // indirect, x
        cpu->regs.a += LOAD8(LOAD16(ARG8 + cpu->regs.x));
        break ;
    case 0x71: // indirect, y
        cpu->regs.a += LOAD8(LOAD16(ARG8) + cpu->regs.y);
        break ;
    }
    if (cpu->regs.a == 0)
        cpu->regs.z = 1;
    if (cpu->regs.a & 0x80)
        cpu->regs.n = 1;
}

void
_call_ror (struct cpu * cpu, uint8_t op)
{
    switch (op) {
    case 0x6A: // accumulator
        cpu->regs.c = cpu->regs.a & (0x01);
        cpu->regs.a >>= 1;
        cpu->regs.a |= (cpu->regs.c << 7);
        break ;
    case 0x66: // zero page
        cpu->regs.c = LOAD8(ARG8) & (0x01);
        LOAD8(ARG8) >>= 1;
        LOAD8(ARG8) |= (cpu->regs.c << 7);
        break ;
    case 0x76: // zero page, x
        cpu->regs.c = LOAD8(ARG8 + cpu->regs.x) & (0x01);
        LOAD8(ARG8 + cpu->regs.x) >>= 1;
        LOAD8(ARG8 + cpu->regs.x) |= (cpu->regs.c << 7);
        break ;
    case 0x6E: // absolute
        cpu->regs.c = LOAD8(ARG16) & (0x01);
        LOAD8(ARG16) >>= 1;
        LOAD8(ARG16) |= (cpu->regs.c << 7);
        break ;
    case 0x7E: // absolute, x
        cpu->regs.c = LOAD8(ARG16 + cpu->regs.x) & (0x01);
        LOAD8(ARG16 + cpu->regs.x) >>= 1;
        LOAD8(ARG16 + cpu->regs.x) |= (cpu->regs.c << 7);
        break ;
    }
}

void
_call_pla (struct cpu * cpu, uint8_t op)
{
    cpu->regs.a = cpu->mem[0x100 + cpu->regs.s];
    cpu->regs.s++;
}

void
_call_bvs (struct cpu * cpu, uint8_t op)
{
    if (cpu->regs.n == 1)
        cpu->regs.new_pc += (int8_t)ARG8;
}

void
_call_sei (struct cpu * cpu, uint8_t op)
{
    cpu->regs.i = 1;
}

void
_call_sta (struct cpu * cpu, uint8_t op)
{
    switch (op) {
    case 0x85: // zero page
        STORE8(ARG8, cpu->regs.a);
        break ;
    case 0x95: // zero page, x
        STORE8(ARG8 + cpu->regs.x, cpu->regs.a);
        break ;
    case 0x8D: // absolute
        STORE8(ARG16, cpu->regs.a);
        break ;
    case 0x9D: // absolute, x
        STORE8(ARG16 + cpu->regs.x, cpu->regs.a);
        break ;
    case 0x99: // absolute, y
        STORE8(ARG16 + cpu->regs.y, cpu->regs.a);
        break ;
    case 0x81: // indirect, x (indexed indirect)
        STORE8(LOAD16(ARG8 + cpu->regs.x), cpu->regs.a);
        break ;
    case 0x91: // indirect, y (indirect indexed)
        STORE8(LOAD16(ARG8) + cpu->regs.y, cpu->regs.a);
        break ;
    }
}

void
_call_stx (struct cpu * cpu, uint8_t op)
{
    switch (op) {
    case 0x86: // zero page
        STORE8(ARG8, cpu->regs.x);
        break ;
    case 0x96: // zero page, y
        STORE8(ARG8 + cpu->regs.y, cpu->regs.x);
        break ;
    case 0x8E: // absolute
        STORE8(ARG16, cpu->regs.x);
        break ;
    }
}

void
_call_sty (struct cpu * cpu, uint8_t op)
{
    switch (op) {
    case 0x86: // zero page
        STORE8(ARG8, cpu->regs.y);
        break ;
    case 0x96: // zero page, x
        STORE8(ARG8 + cpu->regs.x, cpu->regs.y);
        break ;
    case 0x8E: // absolute
        STORE8(ARG16, cpu->regs.y);
        break ;
    }
}

void
_call_dey (struct cpu * cpu, uint8_t op)
{
    cpu->regs.y--;
}

void
_call_txa (struct cpu * cpu, uint8_t op)
{
    cpu->regs.a = cpu->regs.x;
}

void
_call_tya (struct cpu * cpu, uint8_t op)
{
    cpu->regs.a = cpu->regs.y;
}

void
_call_bcc (struct cpu * cpu, uint8_t op)
{
    if (cpu->regs.c == 0)
        cpu->regs.new_pc += (int8_t)ARG8;
}

void
_call_txs (struct cpu * cpu, uint8_t op)
{
    cpu->regs.s = cpu->regs.x;
}

void
_call_lda (struct cpu * cpu, uint8_t op)
{
    switch (op) {
    case 0xA9: // immediate
        cpu->regs.a = ARG8;
        break ;
    case 0xA5: // zero page
        cpu->regs.a = LOAD8(ARG8);
        break ;
    case 0xB5: // zero page, x
        cpu->regs.a = LOAD8(ARG8 + cpu->regs.x);
        break ;
    case 0xAD: // absolute
        cpu->regs.a = LOAD8(ARG16);
        break ;
    case 0xBD: // absolute, x
        cpu->regs.a = LOAD8(ARG16 + cpu->regs.x);
        break ;
    case 0xB9: // absolute, y
        cpu->regs.a = LOAD8(ARG16 + cpu->regs.y);
        break ;
    case 0xA1: // indirect, x (indexed indirect)
        cpu->regs.a = LOAD8(LOAD16(ARG8 + cpu->regs.x));
        break ;
    case 0xB1: // indirect, y (indirect indexed)
        cpu->regs.a = LOAD8(LOAD16(ARG8) + cpu->regs.y);
        break ;
    }
    if (cpu->regs.a == 0)
        cpu->regs.z = 1;
    if (cpu->regs.a & 0x80)
        cpu->regs.n = 1;
}

void
_call_ldx (struct cpu * cpu, uint8_t op)
{
    switch (op) {
    case 0xA2: // immediate
        cpu->regs.x = ARG8;
        break ;
    case 0xA6: // zero page
        cpu->regs.x = LOAD8(ARG8);
        break ;
    case 0xB6: // zero page, y
        cpu->regs.x = LOAD8(ARG8 + cpu->regs.y);
        break ;
    case 0xAE: // absolute
        cpu->regs.x = LOAD8(ARG16);
        break ;
    case 0xBE: // absolute, y
        cpu->regs.x = LOAD8(ARG16 + cpu->regs.y);
        break ;
    }
    if (cpu->regs.x == 0)
        cpu->regs.z = 1;
    if (cpu->regs.x & 0x80)
        cpu->regs.n = 1;
}

void
_call_ldy (struct cpu * cpu, uint8_t op)
{
    switch (op) {
    case 0xA0: // immediate
        cpu->regs.y = ARG8;
        break ;
    case 0xA4: // zero page
        cpu->regs.y = LOAD8(ARG8);
        break ;
    case 0xB4: // zero page, x
        cpu->regs.y = LOAD8(ARG8 + cpu->regs.x);
        break ;
    case 0xAC: // absolute
        cpu->regs.y = LOAD8(ARG16);
        break ;
    case 0xBC: // absolute, x
        cpu->regs.y = LOAD8(ARG16 + cpu->regs.x);
        break ;
    }
    if (cpu->regs.y == 0)
        cpu->regs.z = 1;
    if (cpu->regs.y & 0x80)
        cpu->regs.n = 1;
}

void
_call_tay (struct cpu * cpu, uint8_t op)
{
    cpu->regs.y = cpu->regs.a;
}

void
_call_tax (struct cpu * cpu, uint8_t op)
{
    cpu->regs.a = cpu->regs.x;
}

void
_call_bcs (struct cpu * cpu, uint8_t op)
{
    if (cpu->regs.c == 1)
        cpu->regs.new_pc += (int8_t)ARG8;
}

void
_call_clv (struct cpu * cpu, uint8_t op)
{
    cpu->regs.v = 0;
}

void
_call_tsx (struct cpu * cpu, uint8_t op)
{
    cpu->regs.x = cpu->regs.s;
}

void
_call_cpy (struct cpu * cpu, uint8_t op)
{
    int result;

    switch (op) {
    case 0xE0: // immediate
        result = cpu->regs.x - ARG8;
        break ;
    case 0xE4: // zero page
        result = cpu->regs.x - LOAD8(ARG8);
        break ;
    case 0xEC: // absolute
        result = cpu->regs.x - LOAD8(ARG16);
        break ;
    }
    if (result < 0) {
        cpu->regs.n = 1;
        cpu->regs.z = 0;
        cpu->regs.c = 0;
    }
    else if (result == 0) {
        cpu->regs.n = 0;
        cpu->regs.z = 1;
        cpu->regs.c = 1;
    } else {
        cpu->regs.n = 0;
        cpu->regs.z = 0;
        cpu->regs.c = 1;
    }
}

void
_call_cmp (struct cpu * cpu, uint8_t op)
{
    int result;

    switch (op) {
    case 0xC9: // immediate
        result = cpu->regs.a - ARG8;
        break ;
    case 0xC5: // zero page
        result = cpu->regs.a - LOAD8(ARG8);
        break ;
    case 0xD5: // zero page, x
        result = cpu->regs.a - LOAD8(ARG8 + cpu->regs.x);
        break ;
    case 0xCD: // absolute
        result = cpu->regs.a - LOAD8(ARG16);
        break ;
    case 0xDD: // absolute, x
        result = cpu->regs.a - LOAD8(ARG16 + cpu->regs.x);
        break ;
    case 0xD9: // absolute, y
        result = cpu->regs.a - LOAD8(ARG16 + cpu->regs.y);
    case 0x81: // indirect, x (indexed indirect)
        result = cpu->regs.a - LOAD8(LOAD16(ARG8 + cpu->regs.x));
        break ;
    case 0x91: // indirect, y (indirect indexed)
        result = cpu->regs.a - LOAD8(LOAD16(ARG8) + cpu->regs.y);
        break ;
    }
    if (result < 0) {
        cpu->regs.n = 1;
        cpu->regs.z = 0;
        cpu->regs.c = 0;
    }
    else if (result == 0) {
        cpu->regs.n = 0;
        cpu->regs.z = 1;
        cpu->regs.c = 1;
    } else {
        cpu->regs.n = 0;
        cpu->regs.z = 0;
        cpu->regs.c = 1;
    }
}

void
_call_dec (struct cpu * cpu, uint8_t op)
{
    switch (op) {
    case 0xC6: // zero page
        STORE8(ARG8, LOAD8(ARG8) - 1);
        break ;
    case 0xD6: // zero page, x
        STORE8(ARG8 + cpu->regs.x, LOAD8(ARG8 + cpu->regs.x) - 1);
        break ;
    case 0xCE: // absolute
        STORE8(ARG16, LOAD8(ARG16) - 1);
        break ;
    case 0xDE: // absolute, x
        STORE8(ARG16 + cpu->regs.x, LOAD8(ARG16 + cpu->regs.x) - 1);
        break ;
    }
}

void
_call_iny (struct cpu * cpu, uint8_t op)
{
    cpu->regs.y++;
}

void
_call_dex (struct cpu * cpu, uint8_t op)
{
    cpu->regs.x--;
}

void
_call_bne (struct cpu * cpu, uint8_t op)
{
    if (cpu->regs.z == 0)
        cpu->regs.new_pc += (int8_t)ARG8;
}

void
_call_cld (struct cpu * cpu, uint8_t op)
{
    cpu->regs.d = 0;
}

void
_call_cpx (struct cpu * cpu, uint8_t op)
{
    int result;

    switch (op) {
    case 0xE0: // immediate
        result = cpu->regs.x - ARG8;
        break ;
    case 0xE4: // zero page
        result = cpu->regs.x - LOAD8(ARG8);
        break ;
    case 0xEC: // absolute
        result = cpu->regs.x - LOAD8(ARG16);
        break ;
    }
    if (result < 0) {
        cpu->regs.n = 1;
        cpu->regs.z = 0;
        cpu->regs.c = 0;
    }
    else if (result == 0) {
        cpu->regs.n = 0;
        cpu->regs.z = 1;
        cpu->regs.c = 1;
    } else {
        cpu->regs.n = 0;
        cpu->regs.z = 0;
        cpu->regs.c = 1;
    }
}

void
_call_sbc (struct cpu * cpu, uint8_t op)
{
    switch (op) {
    case 0xE9: // immediate
        cpu->regs.a -= ARG8;
        break ;
    case 0xE5: // zero page
        cpu->regs.a -= LOAD8(ARG8);
        break ;
    case 0xF5: // zero page, x
        cpu->regs.a -= LOAD8(ARG8 + cpu->regs.x);
        break ;
    case 0xED: // absolute
        cpu->regs.a -= LOAD8(ARG16);
        break ;
    case 0xFD: // absolute, x
        cpu->regs.a -= LOAD8(ARG16 + cpu->regs.x);
        break ;
    case 0xF9: // absolute, y
        cpu->regs.a -= LOAD8(ARG16 + cpu->regs.y);
        break ;
    case 0xE1: // indirect, x
        cpu->regs.a -= LOAD8(LOAD16(ARG8 + cpu->regs.x));
        break ;
    case 0xF1: // indirect, y
        cpu->regs.a -= LOAD8(LOAD16(ARG8) + cpu->regs.y);
        break ;
    }
    if (cpu->regs.a == 0)
        cpu->regs.z = 1;
    if (cpu->regs.a & 0x80)
        cpu->regs.n = 1;
}

void
_call_inc (struct cpu * cpu, uint8_t op)
{
    switch (op) {
    case 0xE6: // zero page
        STORE8(ARG8, LOAD8(ARG8) + 1);
        break ;
    case 0xF6: // zero page, x
        STORE8(ARG8 + cpu->regs.x, LOAD8(ARG8 + cpu->regs.x) + 1);
        break ;
    case 0xEE: // absolute
        STORE8(ARG16, LOAD8(ARG16) + 1);
        break ;
    case 0xFE: // absolute, x
        STORE8(ARG16 + cpu->regs.x, LOAD8(ARG16 + cpu->regs.x) + 1);
        break ;
    }
}

void
_call_inx (struct cpu * cpu, uint8_t op)
{
    cpu->regs.x++;
}

void
_call_nop (struct cpu * cpu, uint8_t op)
{
    return ;
}

void
_call_beq (struct cpu * cpu, uint8_t op)
{
    if (cpu->regs.z == 1)
        cpu->regs.new_pc += (int8_t)ARG8;
}

void
_call_sed (struct cpu * cpu, uint8_t op)
{
    cpu->regs.d = 1;
}

void
_call_none (struct cpu * cpu, uint8_t op)
{
    printf("Unknown operand\n");
    exit(1);
}

void
nes_cpu_init (struct nes * nes,
              struct cpu * cpu)
{
    unsigned int        p;
    unsigned int        i;

    memset (cpu->mem, 0xFF, 0x2000);
    for (p = 0; p < 4; p++) {
        i = p * 0x800;

        cpu->mem[0x008 + i] = 0xF7;
        cpu->mem[0x009 + i] = 0xEF;
        cpu->mem[0x00A + i] = 0xDF;
        cpu->mem[0x00F + i] = 0xBF;
    }
    memset (&cpu->mem[0x2001], 0x00, sizeof (cpu->mem) - 0x2000);

    if (nes->header.prg_rom_size == 1) {
        memcpy (&cpu->mem[0xC000], nes->prg_rom, 16384);
        cpu->regs.pc = 0xC000;
    } else {
        memcpy (&cpu->mem[0x8000], nes->prg_rom, nes->header.prg_rom_size * 16384);
        cpu->regs.pc = 0x8000;
    }
    cpu->regs.a = 0;
    cpu->regs.x = 0;
    cpu->regs.y = 0;
    cpu->regs.s = 0xFF;
    cpu->regs.p = 0x28;

    cpu->regs.new_pc = cpu->regs.pc;
}

int
nes_cpu_exec (struct nes * nes,
              struct cpu * cpu,
              uint32_t     options)
{
    struct _opcode {
        char *  name;
        void    (*call)(struct cpu *, uint8_t);
        uint8_t len;
        uint8_t time;
    } opcodes[] = {
        {"BRK",     _call_brk,  1,      7}, // 0x00
        {"ORA",     _call_ora,  2,      6}, // 0x01
        {"NONE",    _call_none, 1,      0}, // 0x02
        {"NONE",    _call_none, 1,      0}, // 0x03
        {"NONE",    _call_none, 1,      0}, // 0x04
        {"ORA",     _call_ora,  2,      3}, // 0x05
        {"ASL",     _call_asl,  2,      5}, // 0x06
        {"NONE",    _call_none, 1,      0}, // 0x07
        {"PHP",     _call_php,  1,      3}, // 0x08
        {"ORA",     _call_ora,  2,      2}, // 0x09
        {"ASL",     _call_asl,  1,      2}, // 0x0A
        {"NONE",    _call_none, 1,      0}, // 0x0B
        {"NONE",    _call_none, 1,      0}, // 0x0C
        {"ORA",     _call_ora,  3,      4}, // 0x0D
        {"ASL",     _call_asl,  3,      6}, // 0x0E
        {"NONE",    _call_none, 1,      0}, // 0x0F
        {"BPL",     _call_bpl,  2,      0}, // 0x10
        {"ORA",     _call_ora,  2,      5}, // 0x11
        {"NONE",    _call_none, 1,      0}, // 0x12
        {"NONE",    _call_none, 1,      0}, // 0x13
        {"NONE",    _call_none, 1,      0}, // 0x14
        {"ORA",     _call_ora,  2,      4}, // 0x15
        {"ASL",     _call_asl,  2,      6}, // 0x16
        {"NONE",    _call_none, 1,      0}, // 0x17
        {"CLC",     _call_clc,  1,      2}, // 0x18
        {"ORA",     _call_ora,  3,      4}, // 0x19
        {"NONE",    _call_none, 1,      0}, // 0x1A
        {"NONE",    _call_none, 1,      0}, // 0x1B
        {"NONE",    _call_none, 1,      0}, // 0x1C
        {"ORA",     _call_ora,  3,      4}, // 0x1D
        {"ASL",     _call_asl,  3,      7}, // 0x1E
        {"NONE",    _call_none, 1,      0}, // 0x1F
        {"JSR",     _call_jsr,  3,      6}, // 0x20
        {"AND",     _call_and,  2,      6}, // 0x21
        {"NONE",    _call_none, 1,      0}, // 0x22
        {"NONE",    _call_none, 1,      0}, // 0x23
        {"BIT",     _call_bit,  2,      3}, // 0x24
        {"AND",     _call_and,  2,      3}, // 0x25
        {"ROL",     _call_rol,  2,      5}, // 0x26
        {"NONE",    _call_none, 1,      0}, // 0x27
        {"PLP",     _call_plp,  1,      4}, // 0x28
        {"AND",	    _call_and,  2,      2}, // 0x29
        {"ROL",     _call_rol,  1,      2}, // 0x2A
        {"NONE",    _call_none, 1,      0}, // 0x2B
        {"BIT",     _call_bit,  3,      4}, // 0x2C
        {"AND",	    _call_and,  3,      4}, // 0x2D
        {"ROL",     _call_rol,  3,      6}, // 0x2E
        {"NONE",    _call_none, 1,      0}, // 0x2F
        {"BMI",     _call_bmi,  2,      0}, // 0x30
        {"AND",	    _call_and,  2,      5}, // 0x31
        {"NONE",    _call_none, 1,      0}, // 0x32
        {"NONE",    _call_none, 1,      0}, // 0x33
        {"NONE",    _call_none, 1,      0}, // 0x34
        {"AND",	    _call_and,  2,      4}, // 0x35
        {"ROL",     _call_rol,  2,      6}, // 0x36
        {"NONE",    _call_none, 1,      0}, // 0x37
        {"SEC",     _call_sec,  1,      2}, // 0x38
        {"AND",	    _call_and,  3,      4}, // 0x39
        {"NONE",    _call_none, 1,      0}, // 0x3A
        {"NONE",    _call_none, 1,      0}, // 0x3B
        {"NONE",    _call_none, 1,      0}, // 0x3C
        {"AND",	    _call_and,  3,      4}, // 0x3D
        {"ROL",     _call_rol,  3,      7}, // 0x3E
        {"NONE",    _call_none, 1,      0}, // 0x3F
        {"RTI",     _call_rti,  1,      6}, // 0x40
        {"EOR",     _call_eor,  2,      6}, // 0x41
        {"NONE",    _call_none, 1,      0}, // 0x42
        {"NONE",    _call_none, 1,      0}, // 0x43
        {"NONE",    _call_none, 1,      0}, // 0x44
        {"EOR",     _call_eor,  2,      3}, // 0x45
        {"LSR",     _call_lsr,  2,      5}, // 0x46
        {"NONE",    _call_none, 1,      0}, // 0x47
        {"PHA",     _call_pha,  1,      3}, // 0x48
        {"EOR",     _call_eor,  2,      2}, // 0x49
        {"LSR",     _call_lsr,  1,      2}, // 0x4A
        {"NONE",    _call_none, 1,      0}, // 0x4B
        {"JMP",     _call_jmp,  3,      3}, // 0x4C
        {"EOR",     _call_eor,  3,      4}, // 0x4D
        {"LSR",     _call_lsr,  3,      6}, // 0x4E
        {"NONE",    _call_none, 1,      0}, // 0x4F
        {"BVC",     _call_bvc,  2,      0}, // 0x50
        {"EOR",     _call_eor,  2,      5}, // 0x51
        {"NONE",    _call_none, 1,      0}, // 0x52
        {"NONE",    _call_none, 1,      0}, // 0x53
        {"NONE",    _call_none, 1,      0}, // 0x54
        {"EOR",     _call_eor,  2,      4}, // 0x55
        {"LSR",     _call_lsr,  2,      6}, // 0x56
        {"NONE",    _call_none, 1,      0}, // 0x57
        {"CLI",     _call_cli,  1,      2}, // 0x58
        {"EOR",     _call_eor,  3,      4}, // 0x59
        {"NONE",    _call_none, 1,      0}, // 0x5A
        {"NONE",    _call_none, 1,      0}, // 0x5B
        {"NONE",    _call_none, 1,      0}, // 0x5C
        {"EOR",     _call_eor,  3,      4}, // 0x5D
        {"LSR",     _call_lsr,  3,      7}, // 0x5E
        {"NONE",    _call_none, 1,      0}, // 0x5F
        {"RTS",     _call_rts,  1,      6}, // 0x60
        {"ADC",     _call_adc,  2,      6}, // 0x61
        {"NONE",    _call_none, 1,      0}, // 0x62
        {"NONE",    _call_none, 1,      0}, // 0x63
        {"NONE",    _call_none, 1,      0}, // 0x64
        {"ADC",     _call_adc,  2,      3}, // 0x65
        {"ROR",     _call_ror,  2,      5}, // 0x66
        {"NONE",    _call_none, 1,      0}, // 0x67
        {"PLA",     _call_pla,  1,      4}, // 0x68
        {"ADC",     _call_adc,  2,      2}, // 0x69
        {"ROR",     _call_ror,  1,      2}, // 0x6A
        {"NONE",    _call_none, 1,      0}, // 0x6B
        {"JMP",     _call_jmp,  3,      5}, // 0x6C
        {"ADC",     _call_adc,  3,      4}, // 0x6D
        {"ROR",     _call_ror,  3,      6}, // 0x6E
        {"NONE",    _call_none, 1,      0}, // 0x6F
        {"BVS",     _call_bvs,  2,      0}, // 0x70
        {"ADC",     _call_adc,  2,      5}, // 0x71
        {"NONE",    _call_none, 1,      0}, // 0x72
        {"NONE",    _call_none, 1,      0}, // 0x73
        {"NONE",    _call_none, 1,      0}, // 0x74
        {"ADC",     _call_adc,  2,      4}, // 0x75
        {"ROR",     _call_ror,  2,      6}, // 0x76
        {"NONE",    _call_none, 1,      0}, // 0x77
        {"SEI",     _call_sei,  1,      2}, // 0x78
        {"ADC",     _call_adc,  3,      4}, // 0x79
        {"NONE",    _call_none, 1,      0}, // 0x7A
        {"NONE",    _call_none, 1,      0}, // 0x7B
        {"NONE",    _call_none, 1,      0}, // 0x7C
        {"ADC",     _call_adc,  3,      4}, // 0x7D
        {"ROR",     _call_ror,  3,      7}, // 0x7E
        {"NONE",    _call_none, 1,      0}, // 0x7F
        {"NONE",    _call_none, 1,      0}, // 0x80
        {"STA",     _call_sta,  2,      6}, // 0x81
        {"NONE",    _call_none, 1,      0}, // 0x82
        {"NONE",    _call_none, 1,      0}, // 0x83
        {"STY",     _call_sty,  2,      3}, // 0x84
        {"STA",     _call_sta,  2,      3}, // 0x85
        {"STX",     _call_stx,  2,      3}, // 0x86
        {"NONE",    _call_none, 1,      0}, // 0x87
        {"DEY",     _call_dey,  1,      2}, // 0x88
        {"NONE",    _call_none, 1,      0}, // 0x89
        {"TXA",     _call_txa,  1,      2}, // 0x8A
        {"NONE",    _call_none, 1,      0}, // 0x8B
        {"STY",     _call_sty,  3,      4}, // 0x8C
        {"STA",     _call_sta,  3,      4}, // 0x8D
        {"STX",     _call_stx,  3,      4}, // 0x8E
        {"NONE",    _call_none, 1,      0}, // 0x8F
        {"BCC",     _call_bcc,  2,      0}, // 0x90
        {"STA",     _call_sta,  2,      6}, // 0x91
        {"NONE",    _call_none, 1,      0}, // 0x92
        {"NONE",    _call_none, 1,      0}, // 0x93
        {"STY",     _call_sty,  2,      4}, // 0x94
        {"STA",     _call_sta,  2,      4}, // 0x95
        {"STX",     _call_stx,  2,      4}, // 0x96
        {"NONE",    _call_none, 1,      0}, // 0x97
        {"TYA",     _call_tya,  1,      2}, // 0x98
        {"STA",     _call_sta,  3,      5}, // 0x99
        {"TXS",     _call_txs,  1,      2}, // 0x9A
        {"NONE",    _call_none, 1,      0}, // 9x9B
        {"NONE",    _call_none, 1,      0}, // 0x9C
        {"STA",     _call_sta,  3,      5}, // 0x9D
        {"NONE",    _call_none, 1,      0}, // 0x9E
        {"NONE",    _call_none, 1,      0}, // 0x9F
        {"LDY",     _call_ldy,  2,      2}, // 0xA0
        {"LDA",     _call_lda,  2,      6}, // 0xA1
        {"LDX",     _call_ldx,  2,      2}, // 0xA2
        {"NONE",    _call_none, 1,      0}, // 0xA3
        {"LDY",     _call_ldy,  2,      3}, // 0xA4
        {"LDA",     _call_lda,  2,      3}, // 0xA5
        {"LDX",     _call_ldx,  2,      3}, // 0xA6
        {"NONE",    _call_none, 1,      0}, // 0xA7
        {"TAY",     _call_tay,  1,      2}, // 0xA8
        {"LDA",     _call_lda,  2,      2}, // 0xA9
        {"TAX",     _call_tax,  1,      2}, // 0xAA
        {"NONE",    _call_none, 1,      0}, // 0xAB
        {"LDY",     _call_ldy,  3,      4}, // 0xAC
        {"LDA",     _call_lda,  3,      4}, // 0xAD
        {"LDX",     _call_ldx,  3,      4}, // 0xAE
        {"NONE",    _call_none, 1,      0}, // 0xAF
        {"BCS",     _call_bcs,  2,      0}, // 0xB0
        {"LDA",     _call_lda,  2,      5}, // 0xB1
        {"NONE",    _call_none, 1,      0}, // 0xB2
        {"NONE",    _call_none, 1,      0}, // 0xB3
        {"LDY",     _call_ldy,  2,      4}, // 0xB4
        {"LDA",     _call_lda,  2,      4}, // 0xB5
        {"LDX",     _call_ldx,  2,      4}, // 0xB6
        {"NONE",    _call_none, 1,      0}, // 0xB7
        {"CLV",     _call_clv,  1,      2}, // 0xB8
        {"LDA",     _call_lda,  3,      4}, // 0xB9
        {"TSX",     _call_tsx,  1,      2}, // 0xBA
        {"NONE",    _call_none, 1,      0}, // 0xBB
        {"LDY",     _call_ldy,  3,      4}, // 0xBC
        {"LDA",     _call_lda,  3,      4}, // 0xBD
        {"LDX",     _call_ldx,  3,      4}, // 0xBE
        {"NONE",    _call_none, 1,      0}, // 0xBF
        {"CPY",     _call_cpy,  2,      2}, // 0xC0
        {"CMP",     _call_cmp,  2,      6}, // 0xC1
        {"NONE",    _call_none, 1,      0}, // 0xC2
        {"NONE",    _call_none, 1,      0}, // 0xC3
        {"CPY",     _call_cpy,  2,      3}, // 0xC4
        {"CMP",     _call_cmp,  2,      3}, // 0xC5
        {"DEC",     _call_dec,  2,      5}, // 0xC6
        {"NONE",    _call_none, 1,      0}, // 0xC7
        {"INY",     _call_iny,  1,      2}, // 0xC8
        {"CMP",     _call_cmp,  2,      2}, // 0xC9
        {"DEX",     _call_dex,  1,      2}, // 0xCA
        {"NONE",    _call_none, 1,      0}, // 0xCB
        {"CPY",     _call_cpy,  3,      4}, // 0xCC
        {"CMP",     _call_cmp,  3,      4}, // 0xCD
        {"DEC",     _call_dec,  3,      6}, // 0xCE
        {"NONE",    _call_none, 1,      0}, // 0xCF
        {"BNE",     _call_bne,  2,      0}, // 0xD0
        {"CMP",     _call_cmp,  2,      5}, // 0xD1
        {"NONE",    _call_none, 1,      0}, // 0xD2
        {"NONE",    _call_none, 1,      0}, // 0xD3
        {"NONE",    _call_none, 1,      0}, // 0xD4
        {"CMP",     _call_cmp,  2,      4}, // 0xD5
        {"DEC",     _call_dec,  2,      6}, // 0xD6
        {"NONE",    _call_none, 1,      0}, // 0xD7
        {"CLD",     _call_cld,  1,      2}, // 0xD8
        {"CMP",     _call_cmp,  3,      4}, // 0xD9
        {"NONE",    _call_none, 1,      0}, // 0xDA
        {"NONE",    _call_none, 1,      0}, // 0xDB
        {"NONE",    _call_none, 1,      0}, // 0xDC
        {"CMP",     _call_cmp,  3,      4}, // 0xDD
        {"DEC",     _call_dec,  3,      7}, // 0xDE
        {"NONE",    _call_none, 1,      0}, // 0xDF
        {"CPX",     _call_cpx,  2,      2}, // 0xE0
        {"SBC",     _call_sbc,  2,      6}, // 0xE1
        {"NONE",    _call_none, 1,      0}, // 0xE2
        {"NONE",    _call_none, 1,      0}, // 0xE3
        {"CPX",     _call_cpx,  2,      3}, // 0xE4
        {"SBC",     _call_sbc,  2,      3}, // 0xE5
        {"INC",     _call_inc,  2,      5}, // 0xE6
        {"NONE",    _call_none, 1,      0}, // 0xE7
        {"INX",     _call_inx,  1,      2}, // 0xE8
        {"SBC",     _call_sbc,  2,      2}, // 0xE9
        {"NOP",     _call_nop,  1,      2}, // 0xEA
        {"NONE",    _call_none, 1,      0}, // 0xEB
        {"CPX",     _call_cpx,  3,      4}, // 0xEC
        {"SBC",     _call_sbc,  3,      4}, // 0xED
        {"INC",     _call_inc,  3,      6}, // 0xEE
        {"NONE",    _call_none, 1,      0}, // 0xEF
        {"BEQ",     _call_beq,  2,      0}, // 0xF0
        {"SBC",     _call_sbc,  2,      5}, // 0xF1
        {"NONE",    _call_none, 1,      0}, // 0xF2
        {"NONE",    _call_none, 1,      0}, // 0xF3
        {"NONE",    _call_none, 1,      0}, // 0xF4
        {"SBC",     _call_sbc,  2,      4}, // 0xF5
        {"INC",     _call_inc,  2,      6}, // 0xF6
        {"NONE",    _call_none, 1,      0}, // 0xF7
        {"SED",     _call_sed,  1,      2}, // 0xF8
        {"SBC",     _call_sbc,  3,      4}, // 0xF9
        {"NONE",    _call_none, 1,      0}, // 0xFA
        {"NONE",    _call_none, 1,      0}, // 0xFB
        {"NONE",    _call_none, 1,      0}, // 0xFC
        {"SBC",     _call_sbc,  3,      4}, // 0xFD
        {"INC",     _call_inc,  3,      7}, // 0xFE
        {"NONE",    _call_none, 1,      0}, // 0xFF
    };
    uint8_t             op;

    op = cpu->mem[cpu->regs.pc];

    cpu->regs.new_pc += opcodes[op].len;
    opcodes[op].call (cpu, op);

    if (options & NES_DEBUG) {
        printf ("%s(%02x) ", opcodes[op].name, (unsigned char)op);
        if (opcodes[op].len == 2) {
            printf ("%02x", ARG8);
        } else if (opcodes[op].len == 3) {
            printf ("%04x", ARG16);
        }
        printf ("\t(a: %02x, ", cpu->regs.a);
        printf ("x: %02x, ", cpu->regs.x);
        printf ("y: %02x, ", cpu->regs.y);
        printf ("s: %02x, ", cpu->regs.s);
        printf ("c: %01x, ", cpu->regs.c);
        printf ("z: %01x, ", cpu->regs.z);
        printf ("i: %01x, ", cpu->regs.i);
        printf ("d: %01x, ", cpu->regs.d);
        printf ("b: %01x, ", cpu->regs.b);
        printf ("v: %01x, ", cpu->regs.v);
        printf ("n: %01x)\n", cpu->regs.n);
    }

    cpu->regs.pc = cpu->regs.new_pc;
}
