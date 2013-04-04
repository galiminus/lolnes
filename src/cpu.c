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
_call_brk (struct cpu * cpu, uint16_t  param)
{
    cpu->regs.new_pc += 2;
}

void
_call_ora (struct cpu * cpu, uint16_t  param) // OK
{
    cpu->regs.a |= LOAD8(param);

    cpu->regs.z = cpu->regs.a == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.a & 0x80 ? 1 : 0;
}

void
_call_asl (struct cpu *cpu, uint16_t  param)
{
    cpu->regs.c = LOAD8(param) & 0x80;
    LOAD8(param) <<= 1;

    cpu->regs.z = LOAD8(param) == 0 ? 1 : 0;
    cpu->regs.n = LOAD8(param) & 0x80 ? 1 : 0;
}

void
_call_php (struct cpu * cpu, uint16_t  param)
{
    (void) param;

    cpu->mem[0x100 + cpu->regs.s] = cpu->regs.p;
    cpu->regs.s--;
}

void
_call_bpl (struct cpu * cpu, uint16_t  param) // OK
{
    if (cpu->regs.n == 0)
        cpu->regs.new_pc = param;
}

void
_call_clc (struct cpu * cpu, uint16_t  param)
{
    (void) param;

    cpu->regs.c = 0;
}

void
_call_jsr (struct cpu * cpu, uint16_t  param)
{
    uint16_t pc;

    pc = cpu->regs.pc - 1;

    cpu->mem[0x100 + cpu->regs.s]     = pc >> 8;
    cpu->mem[0x100 + cpu->regs.s - 1] = pc & 0x00FF;
    cpu->regs.s -= 2;

    cpu->regs.new_pc = ARG16;
}

void
_call_and (struct cpu * cpu, uint16_t  param)
{
    cpu->regs.a &= LOAD8(param);

    cpu->regs.z = cpu->regs.a == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.a & 0x80 ? 1 : 0;
}

void
_call_bit (struct cpu * cpu, uint16_t  param)
{
    uint8_t value = LOAD8(param);

    cpu->regs.z = (value & cpu->regs.a) == 0 ? 1 : 0;
    cpu->regs.n = value & 0x80 ? 1 : 0;
    cpu->regs.v = value & 0x40 ? 1 : 0;
}

void
_call_rol (struct cpu * cpu, uint16_t  param)
{
    cpu->regs.c = LOAD8(param) & 0x80;
    LOAD8(param) <<= 1;
    LOAD8(param) |= cpu->regs.c;

    cpu->regs.z = LOAD8(param) == 0 ? 1 : 0;
    cpu->regs.n = LOAD8(param) & 0x80 ? 1 : 0;
}


void
_call_plp (struct cpu * cpu, uint16_t  param)
{
    (void) param;

    cpu->regs.s++;
    cpu->regs.p = cpu->mem[0x100 + cpu->regs.s];
}


void
_call_bmi (struct cpu * cpu, uint16_t  param)
{
    if (cpu->regs.n == 1)
        cpu->regs.new_pc = param;
}


void
_call_sec (struct cpu * cpu, uint16_t  param)
{
    (void) param;

    cpu->regs.c = 1;
}


void
_call_rti (struct cpu * cpu, uint16_t  param)
{
}


void
_call_eor (struct cpu * cpu, uint16_t  param)
{
    cpu->regs.a ^= LOAD8(param);

    cpu->regs.z = cpu->regs.a == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.a & 0x80 ? 1 : 0;
}

void
_call_lsr (struct cpu * cpu, uint16_t  param)
{
    cpu->regs.c = LOAD8(param) & 0x01;
    LOAD8(param) >>= 1;
    cpu->regs.z = LOAD8(param) == 0 ? 1 : 0;
    cpu->regs.n = LOAD8(param) & 0x80 ? 1 : 0;
}

void
_call_pha (struct cpu * cpu, uint16_t  param)
{
    (void) param;

    cpu->mem[0x100 + cpu->regs.s] = cpu->regs.a;
    cpu->regs.s--;
}

void
_call_jmp (struct cpu * cpu, uint16_t  param)
{
    cpu->regs.new_pc = LOAD16(param);
}

void
_call_bvc (struct cpu * cpu, uint16_t  param)
{
    if (cpu->regs.v == 0)
        cpu->regs.new_pc = param;
}

void
_call_cli (struct cpu * cpu, uint16_t  param)
{
    cpu->regs.i = 0;
}

void
_call_rts (struct cpu * cpu, uint16_t  param)
{
    (void) param;

    cpu->regs.new_pc = (uint16_t)(cpu->mem[0x100 + cpu->regs.s + 2]) << 8;
    cpu->regs.new_pc |= cpu->mem[0x100 + cpu->regs.s + 1];
    cpu->regs.s += 2;

    cpu->regs.new_pc++;
}

void
_call_adc (struct cpu * cpu, uint16_t  param)
{
    uint32_t    temp;

    temp = cpu->regs.a + LOAD8(param) + cpu->regs.c;
    cpu->regs.v = (!(((cpu->regs.a ^ LOAD8(param))) & 0x80) != 0) &&
        (((cpu->regs.a ^ temp) & 0x80)) != 0 ? 1 : 0;

    cpu->regs.c = temp > 255 ? 1 : 0;
    cpu->regs.z = temp == 0 ? 1 : 0;
    cpu->regs.n = temp & 0x80 ? 1 : 0;

    cpu->regs.a = temp;
}

void
_call_ror (struct cpu * cpu, uint16_t  param)
{
    cpu->regs.c = LOAD8(param) & 0x01;
    LOAD8(param) >>= 1;
    LOAD8(param) |= (cpu->regs.c << 7);

    cpu->regs.z = LOAD8(param) == 0 ? 1 : 0;
    cpu->regs.n = LOAD8(param) & 0x80 ? 1 : 0;
}

void
_call_pla (struct cpu * cpu, uint16_t  param)
{
    (void) param;

    cpu->regs.s++;
    cpu->regs.a = cpu->mem[0x100 + cpu->regs.s];
}

void
_call_bvs (struct cpu * cpu, uint16_t  param)
{
    if (cpu->regs.n == 1)
        cpu->regs.new_pc = param;
}

void
_call_sei (struct cpu * cpu, uint16_t  param)
{
    (void) param;

    cpu->regs.i = 1;
}

void
_call_sta (struct cpu * cpu, uint16_t  param)
{
    STORE8(LOAD8(param), cpu->regs.a);
}

void
_call_stx (struct cpu * cpu, uint16_t  param)
{
    STORE8(LOAD8(param), cpu->regs.x);
}

void
_call_sty (struct cpu * cpu, uint16_t  param)
{
    STORE8(LOAD8(param), cpu->regs.y);
}

void
_call_dey (struct cpu * cpu, uint16_t  param)
{
    (void) param;

    cpu->regs.y--;

    cpu->regs.z = cpu->regs.y == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.y & 0x80 ? 1 : 0;
}

void
_call_txa (struct cpu * cpu, uint16_t  param)
{
    (void) param;

    cpu->regs.a = cpu->regs.x;

    cpu->regs.z = cpu->regs.a == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.a & 0x80 ? 1 : 0;
}

void
_call_tya (struct cpu * cpu, uint16_t  param)
{
    (void) param;

    cpu->regs.a = cpu->regs.y;

    cpu->regs.z = cpu->regs.a == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.a & 0x80 ? 1 : 0;
}

void
_call_bcc (struct cpu * cpu, uint16_t  param)
{
    if (cpu->regs.c == 0)
        cpu->regs.new_pc = param;
}

void
_call_txs (struct cpu * cpu, uint16_t  param)
{
    (void) param;

    cpu->regs.s = cpu->regs.x;
}

void
_call_lda (struct cpu * cpu, uint16_t  param)
{
    cpu->regs.a = LOAD8(param);

    cpu->regs.z = cpu->regs.a == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.a & 0x80 ? 1 : 0;
}

void
_call_ldx (struct cpu * cpu, uint16_t  param) // OK
{
    cpu->regs.x = LOAD8(param);

    cpu->regs.z = cpu->regs.x == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.x & 0x80 ? 1 : 0;
}

void
_call_ldy (struct cpu * cpu, uint16_t  param) // OK
{
    cpu->regs.y = LOAD8(param);

    cpu->regs.z = cpu->regs.y == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.y & 0x80 ? 1 : 0;
}

void
_call_tay (struct cpu * cpu, uint16_t  param)
{
    (void) param;

    cpu->regs.y = cpu->regs.a;

    cpu->regs.z = cpu->regs.y == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.y & 0x80 ? 1 : 0;
}

void
_call_tax (struct cpu * cpu, uint16_t  param)
{
    (void) param;

    cpu->regs.x = cpu->regs.a;

    cpu->regs.z = cpu->regs.x == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.x & 0x80 ? 1 : 0;
}

void
_call_bcs (struct cpu * cpu, uint16_t  param)
{
    if (cpu->regs.c == 1)
        cpu->regs.new_pc = param;
}

void
_call_clv (struct cpu * cpu, uint16_t  param)
{
    (void) param;

    cpu->regs.v = 0;
}

void
_call_tsx (struct cpu * cpu, uint16_t  param)
{
    (void) param;

    cpu->regs.x = cpu->regs.s;
}

void
_call_cpy (struct cpu * cpu, uint16_t  param)
{
    int result = cpu->regs.x - LOAD8(param);

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
_call_cmp (struct cpu * cpu, uint16_t  param)
{
    int result = cpu->regs.a - LOAD8(param);

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
_call_dec (struct cpu * cpu, uint16_t  param)
{
    LOAD8(param) -= 1;

    cpu->regs.z = LOAD8(param) == 0 ? 1 : 0;
    cpu->regs.n = LOAD8(param) & 0x80 ? 1 : 0;
}

void
_call_iny (struct cpu * cpu, uint16_t  param)
{
    (void) param;

    cpu->regs.y++;

    cpu->regs.z = cpu->regs.y == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.y & 0x80 ? 1 : 0;
}

void
_call_dex (struct cpu * cpu, uint16_t  param)
{
    (void) param;

    cpu->regs.x--;

    cpu->regs.z = cpu->regs.x == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.x & 0x80 ? 1 : 0;
}

void
_call_bne (struct cpu * cpu, uint16_t  param)
{
    if (cpu->regs.z == 0)
        cpu->regs.new_pc = param;
}

void
_call_cld (struct cpu * cpu, uint16_t  param)
{
    (void) param;

    cpu->regs.d = 0;
}

void
_call_cpx (struct cpu * cpu, uint16_t  param)
{
    int result = cpu->regs.x - LOAD8(param);

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
_call_sbc (struct cpu * cpu, uint16_t  param)
{
    int32_t    temp;

    temp = cpu->regs.a - LOAD8(param) - (1 - cpu->regs.c);

    cpu->regs.v = (((cpu->regs.a ^ temp) & 0x80) != 0 &&
                   ((cpu->regs.a ^ LOAD8(param)) & 0x80) != 0) ? 1 : 0;
    cpu->regs.c = temp < 0 ? 1 : 0;
    cpu->regs.z = temp == 0 ? 1 : 0;
    cpu->regs.n = temp & 0x80 ? 1 : 0;
}

void
_call_inc (struct cpu * cpu, uint16_t  param)
{
    LOAD8(param) += 1;

    cpu->regs.z = LOAD8(param) == 0 ? 1 : 0;
    cpu->regs.n = LOAD8(param) & 0x80 ? 1 : 0;
}

void
_call_inx (struct cpu * cpu, uint16_t  param)
{
    (void) param;

    cpu->regs.x++;

    cpu->regs.z = cpu->regs.x == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.x & 0x80 ? 1 : 0;
}

void
_call_nop (struct cpu * cpu, uint16_t  param)
{
    (void) param;
    return ;
}

void
_call_beq (struct cpu * cpu, uint16_t  param)
{
    if (cpu->regs.z == 1)
        cpu->regs.new_pc = param;
}

void
_call_sed (struct cpu * cpu, uint16_t  param)
{
    (void) param;

    cpu->regs.d = 1;
}

void
_call_non (struct cpu * cpu, uint16_t  param)
{
    (void) param;

    exit(1);
}

void
nes_cpu_init (struct nes * nes,
              struct cpu * cpu)
{
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
    uint16_t            param;

    op = cpu->mem[cpu->regs.pc];
    switch (opcodes[op].addr_mode) {
    case ADDR_MODE_ZPA:  // zero page mode
        param = ARG8;                                   break ;
    case ADDR_MODE_REL:  // relative mode
        param = cpu->regs.pc + (int8_t)ARG8;            break ;
    case ADDR_MODE_ABS:  // absolute
        param = ARG16;                                  break ;
    case ADDR_MODE_ACC:  // accumulator
        param = cpu->regs.a;                            break ;
    case ADDR_MODE_IMM:  // immediate mode
        param = cpu->regs.pc + 1;                       break ;
    case ADDR_MODE_ZPX:  // zero page, x
        param = ARG8 + cpu->regs.x;                     break ;
    case ADDR_MODE_ZPY:  // zero page, y
        param = ARG8 + cpu->regs.y;                     break ;
    case ADDR_MODE_ABX:  // absolute, x
        param = ARG16 + cpu->regs.x;                    break ;
    case ADDR_MODE_ABY:  // absolute, y
        param = ARG16 + cpu->regs.y;                    break ;
    case ADDR_MODE_INX: // indirect, x (indexed indirect)
        param = LOAD16(ARG8 + cpu->regs.x);             break ;
    case ADDR_MODE_INY: // indirect, y (indirect indexed)
        param = LOAD16(ARG8) + cpu->regs.y;             break ;
    case ADDR_MODE_IAB: // indirect absolute
        param = LOAD16(ARG16);                          break ;
    default:
        param = 0;
    }

    cpu->regs.new_pc += opcodes[op].len;
    opcodes[op].call (cpu, param);

    if (cpu->debug.checkpoint == cpu->regs.pc) {
        cpu->debug.checkpoint = 0xFFFF;
    }
    if (cpu->debug.run > 0) {
        cpu->debug.run--;
    }
    if (options & NES_DEBUG && cpu->debug.checkpoint == 0xFFFF && cpu->debug.run == 0) {
        printf ("\x1b[32m[%04x>%04x]\x1b[0m[%02x]\t \x1b[31m%s\x1b[0m",
                cpu->regs.pc, cpu->regs.new_pc, (unsigned char)op, opcodes[op].name);
        if (opcodes[op].len == 2) {
            printf ("(\x1b[34m%02x\x1b[0m)  ", ARG8);
        } else if (opcodes[op].len == 3) {
            printf ("(\x1b[34m%04x\x1b[0m)", ARG16);
        } else {
            printf ("      ");
        }

        printf (" %s",
                (char*[]){ "zpa",
                        "rel",
                        "imp",
                        "abs",
                        "acc",
                        "imm",
                        "zpx",
                        "zpy",
                        "abx",
                        "aby",
                        "inx",
                        "iny",
                        "iab"}[opcodes[op].addr_mode]);

        printf ("\t\ta:\x1b[34m%02x\x1b[0m|", cpu->regs.a);
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

    return (0);
}
