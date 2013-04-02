#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "nes.h"
#include "cpu.h"

#include "opcodes.h"

#define ARG8            (cpu)->mem[(cpu)->regs.pc + 1]
#define ARG16           (((uint16_t)((cpu)->mem[(cpu)->regs.pc + 2])) << 8 | ARG8)

#define LOAD8(n)        (cpu)->mem[(n)]
#define LOAD16(n)       ((uint16_t)(LOAD8(n + 1)) << 8 | LOAD8(n))

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
_call_ora (struct cpu * cpu, uint8_t op) // OK
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

    cpu->regs.z = cpu->regs.a == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.a & 0x80 ? 1 : 0;
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

    cpu->regs.z = cpu->regs.a == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.a & 0x80 ? 1 : 0;
}

void
_call_php (struct cpu * cpu, uint8_t op)
{
    cpu->mem[0x100 + cpu->regs.s] = cpu->regs.p;
    cpu->regs.s--;
}

void
_call_bpl (struct cpu * cpu, uint8_t op) // OK
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
    uint16_t pc;

    pc = cpu->regs.pc - 1;

    cpu->mem[0x100 + cpu->regs.s]     = pc >> 8;
    cpu->mem[0x100 + cpu->regs.s - 1] = pc & 0x00FF;
    cpu->regs.s -= 2;

    cpu->regs.new_pc = ARG16;
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

    cpu->regs.z = cpu->regs.a == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.a & 0x80 ? 1 : 0;
}

void
_call_bit (struct cpu * cpu, uint8_t op)
{
    uint8_t value;

    switch (op) {
    case 0x24: // zero page
        value = LOAD8(ARG8);
        break ;
    case 0x2C: // absolute
        value = LOAD8(ARG16);
        break ;
    }

    cpu->regs.z = (value & cpu->regs.a) == 0 ? 1 : 0;
    cpu->regs.n = value & 0x80 ? 1 : 0;
    cpu->regs.v = value & 0x40 ? 1 : 0;
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

    cpu->regs.z = cpu->regs.a == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.a & 0x80 ? 1 : 0;
}


void
_call_plp (struct cpu * cpu, uint8_t op)
{
    cpu->regs.s++;
    cpu->regs.p = cpu->mem[0x100 + cpu->regs.s];
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

    cpu->regs.z = cpu->regs.a == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.a & 0x80 ? 1 : 0;
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

    cpu->regs.z = cpu->regs.a == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.a & 0x80 ? 1 : 0;
}

void
_call_pha (struct cpu * cpu, uint8_t op)
{
    cpu->mem[0x100 + cpu->regs.s] = cpu->regs.a;
    cpu->regs.s--;
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
    cpu->regs.new_pc = (uint16_t)(cpu->mem[0x100 + cpu->regs.s + 2]) << 8;
    cpu->regs.new_pc |= cpu->mem[0x100 + cpu->regs.s + 1];
    cpu->regs.s += 2;

    cpu->regs.new_pc++;
}

void
_call_adc (struct cpu * cpu, uint8_t op)
{
    uint32_t    temp;
    uint8_t     value;

    switch (op) {
    case 0x69: // immediate
        value = ARG8;
        break ;
    case 0x65: // zero page
        value = LOAD8(ARG8);
        break ;
    case 0x75: // zero page, x
        value = LOAD8(ARG8 + cpu->regs.x);
        break ;
    case 0x6D: // absolute
        value = LOAD8(ARG16);
        break ;
    case 0x7D: // absolute, x
        value = LOAD8(ARG16 + cpu->regs.x);
        break ;
    case 0x79: // absolute, y
        value = LOAD8(ARG16 + cpu->regs.y);
        break ;
    case 0x61: // indirect, x
        value = LOAD8(LOAD16(ARG8 + cpu->regs.x));
        break ;
    case 0x71: // indirect, y
        value = LOAD8(LOAD16(ARG8) + cpu->regs.y);
        break ;
    }
    temp = cpu->regs.a + value + cpu->regs.c;

    cpu->regs.v = (!(((cpu->regs.a ^ value)) & 0x80) != 0) && (((cpu->regs.a ^ temp) & 0x80)) != 0 ? 1 : 0;
    cpu->regs.c = temp > 255 ? 1 : 0;
    cpu->regs.z = temp == 0 ? 1 : 0;
    cpu->regs.n = temp & 0x80 ? 1 : 0;

    cpu->regs.a = temp;
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

    cpu->regs.z = cpu->regs.a == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.a & 0x80 ? 1 : 0;
}

void
_call_pla (struct cpu * cpu, uint8_t op)
{
    cpu->regs.s++;
    cpu->regs.a = cpu->mem[0x100 + cpu->regs.s];
}

void
_call_bvs (struct cpu * cpu, uint8_t op)
{
    if (cpu->regs.n == 1)
        cpu->regs.new_pc += (int8_t)ARG8;
}

void
_call_sei (struct cpu * cpu, uint8_t op) // OK
{
    cpu->regs.i = 1;
}

void
_call_sta (struct cpu * cpu, uint8_t op) // OK
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
_call_stx (struct cpu * cpu, uint8_t op) // OK
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
_call_sty (struct cpu * cpu, uint8_t op) // OK
{
    switch (op) {
    case 0x84: // zero page
        STORE8(ARG8, cpu->regs.y);
        break ;
    case 0x94: // zero page, x
        STORE8(ARG8 + cpu->regs.x, cpu->regs.y);
        break ;
    case 0x8C: // absolute
        STORE8(ARG16, cpu->regs.y);
        break ;
    }
}

void
_call_dey (struct cpu * cpu, uint8_t op) // OK
{
    cpu->regs.y--;

    cpu->regs.z = cpu->regs.y == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.y & 0x80 ? 1 : 0;
}

void
_call_txa (struct cpu * cpu, uint8_t op)
{
    cpu->regs.a = cpu->regs.x;

    cpu->regs.z = cpu->regs.a == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.a & 0x80 ? 1 : 0;
}

void
_call_tya (struct cpu * cpu, uint8_t op)
{
    cpu->regs.a = cpu->regs.y;

    cpu->regs.z = cpu->regs.a == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.a & 0x80 ? 1 : 0;
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
_call_lda (struct cpu * cpu, uint8_t op) // OK
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

    cpu->regs.z = cpu->regs.a == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.a & 0x80 ? 1 : 0;
}

void
_call_ldx (struct cpu * cpu, uint8_t op) // OK
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

    cpu->regs.z = cpu->regs.x == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.x & 0x80 ? 1 : 0;
}

void
_call_ldy (struct cpu * cpu, uint8_t op) // OK
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

    cpu->regs.z = cpu->regs.y == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.y & 0x80 ? 1 : 0;
}

void
_call_tay (struct cpu * cpu, uint8_t op)
{
    cpu->regs.y = cpu->regs.a;

    cpu->regs.z = cpu->regs.y == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.y & 0x80 ? 1 : 0;
}

void
_call_tax (struct cpu * cpu, uint8_t op)
{
    cpu->regs.x = cpu->regs.a;

    cpu->regs.z = cpu->regs.x == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.x & 0x80 ? 1 : 0;
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
    case 0xC0: // immediate
        result = cpu->regs.x - ARG8;
        break ;
    case 0xC4: // zero page
        result = cpu->regs.x - LOAD8(ARG8);
        break ;
    case 0xCC: // absolute
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
    case 0xC1: // indirect, x (indexed indirect)
        result = cpu->regs.a - LOAD8(LOAD16(ARG8 + cpu->regs.x));
        break ;
    case 0xD1: // indirect, y (indirect indexed)
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
    uint8_t     value;

    switch (op) {
    case 0xC6: // zero page
        value = LOAD8(ARG8) - 1;
        STORE8(ARG8, value);
        break ;
    case 0xD6: // zero page, x
        value = LOAD8(ARG8 + cpu->regs.x) - 1;
        STORE8(ARG8 + cpu->regs.x, value);
        break ;
    case 0xCE: // absolute
        value = LOAD8(ARG16) - 1;
        STORE8(ARG16, value);
        break ;
    case 0xDE: // absolute, x
        value = LOAD8(ARG16 + cpu->regs.x) - 1;
        STORE8(ARG16 + cpu->regs.x, value);
        break ;
    }

    cpu->regs.z = value == 0 ? 1 : 0;
    cpu->regs.n = value & 0x80 ? 1 : 0;
}

void
_call_iny (struct cpu * cpu, uint8_t op)
{
    cpu->regs.y++;

    cpu->regs.z = cpu->regs.y == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.y & 0x80 ? 1 : 0;
}

void
_call_dex (struct cpu * cpu, uint8_t op)
{
    cpu->regs.x--;

    cpu->regs.z = cpu->regs.x == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.x & 0x80 ? 1 : 0;
}

void
_call_bne (struct cpu * cpu, uint8_t op)
{
    if (cpu->regs.z == 0)
        cpu->regs.new_pc += (int8_t)ARG8;
}

void
_call_cld (struct cpu * cpu, uint8_t op) // OK
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
    int32_t    temp;
    uint8_t     value;

    switch (op) {
    case 0xE9: // immediate
        value = ARG8;
        break ;
    case 0xE5: // zero page
        value = LOAD8(ARG8);
        break ;
    case 0xF5: // zero page, x
        value = LOAD8(ARG8 + cpu->regs.x);
        break ;
    case 0xED: // absolute
        value = LOAD8(ARG16);
        break ;
    case 0xFD: // absolute, x
        value = LOAD8(ARG16 + cpu->regs.x);
        break ;
    case 0xF9: // absolute, y
        value = LOAD8(ARG16 + cpu->regs.y);
        break ;
    case 0xE1: // indirect, x
        value = LOAD8(LOAD16(ARG8 + cpu->regs.x));
        break ;
    case 0xF1: // indirect, y
        value = LOAD8(LOAD16(ARG8) + cpu->regs.y);
        break ;
    }
    temp = cpu->regs.a - value - (1 - cpu->regs.c);

    cpu->regs.v = (((cpu->regs.a ^ temp) & 0x80) != 0 && ((cpu->regs.a ^ value) & 0x80) != 0) ? 1 : 0;
    cpu->regs.c = temp < 0 ? 1 : 0;
    cpu->regs.z = temp == 0 ? 1 : 0;
    cpu->regs.n = temp & 0x80 ? 1 : 0;
}

void
_call_inc (struct cpu * cpu, uint8_t op)
{
    uint8_t     value;

    switch (op) {
    case 0xE6: // zero page
        value = LOAD8(ARG8) + 1;
        STORE8(ARG8, value);
        break ;
    case 0xF6: // zero page, x
        value = LOAD8(ARG8 + cpu->regs.x) + 1;
        STORE8(ARG8 + cpu->regs.x, value);
        break ;
    case 0xEE: // absolute
        value = LOAD8(ARG16) + 1;
        STORE8(ARG16, value);
        break ;
    case 0xFE: // absolute, x
        value = LOAD8(ARG16 + cpu->regs.x) + 1;
        STORE8(ARG16 + cpu->regs.x, value);
        break ;
    }

    cpu->regs.z = value == 0 ? 1 : 0;
    cpu->regs.n = value & 0x80 ? 1 : 0;
}

void
_call_inx (struct cpu * cpu, uint8_t op)
{
    cpu->regs.x++;

    cpu->regs.z = cpu->regs.x == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.x & 0x80 ? 1 : 0;
}

void
_call_nop (struct cpu * cpu, uint8_t op) // OK
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
_call_sed (struct cpu * cpu, uint8_t op) // OK
{
    cpu->regs.d = 1;
}

void
_call_none (struct cpu * cpu, uint8_t op) // OK
{
    exit(1);
}

void
nes_cpu_init (struct nes * nes,
              struct cpu * cpu)
{
    unsigned int        p;
    unsigned int        i;

    memset (cpu->mem, 0x00, 0xFFFF);
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
    cpu->debug.checkpoint = 0xFFFF;
    cpu->debug.run = 0;
    cpu->debug.count = 1;
}

int
nes_cpu_exec (struct nes * nes,
              struct cpu * cpu,
              uint32_t     options)
{
    uint8_t             op;

    op = cpu->mem[cpu->regs.pc];

    cpu->regs.new_pc += opcodes[op].len;
    opcodes[op].call (cpu, op);

    if (cpu->debug.checkpoint == cpu->regs.pc) {
        cpu->debug.checkpoint = 0xFFFF;
    }
    if (cpu->debug.run > 0) {
        cpu->debug.run--;
    }
    if (options & NES_DEBUG && cpu->debug.checkpoint == 0xFFFF && cpu->debug.run == 0) {
        printf ("\x1b[32m[%04x>%04x]\x1b[0m\t \x1b[31m%s\x1b[0m[%02x]",
                cpu->regs.pc, cpu->regs.new_pc, opcodes[op].name, (unsigned char)op);
        if (opcodes[op].len == 2) {
            printf ("(\x1b[34m%02x\x1b[0m)", ARG8);
        } else if (opcodes[op].len == 3) {
            printf ("(\x1b[34m%04x\x1b[0m)", ARG16);
        }
        printf ("\ta:\x1b[34m%02x\x1b[0m|", cpu->regs.a);
        printf ("x:\x1b[34m%02x\x1b[0m|", cpu->regs.x);
        printf ("y:\x1b[34m%02x\x1b[0m|", cpu->regs.y);
        printf ("s:\x1b[34m%02x\x1b[0m|", cpu->regs.s);
        printf ("c:\x1b[34m%01x\x1b[0m|", cpu->regs.c);
        printf ("z:\x1b[34m%01x\x1b[0m|", cpu->regs.z);
        printf ("i:\x1b[34m%01x\x1b[0m|", cpu->regs.i);
        printf ("d:\x1b[34m%01x\x1b[0m|", cpu->regs.d);
        printf ("b:\x1b[34m%01x\x1b[0m|", cpu->regs.b);
        printf ("v:\x1b[34m%01x\x1b[0m|", cpu->regs.v);
        printf ("n:\x1b[34m%01x\x1b[0m", cpu->regs.n);

        printf("\t\t- %d\n", cpu->debug.count);
    }

    cpu->debug.count++;
    cpu->regs.pc = cpu->regs.new_pc;
}
