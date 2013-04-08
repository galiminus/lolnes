#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "nes.h"
#include "cpu.h"

#include "opcodes.h"

uint8_t
_load8 (struct cpu * cpu, uint16_t addr)
{
    uint8_t value = cpu->mem[addr];

    switch (addr) {
    case 0x2002:
        cpu->mem[0x2002] &= 0x7F; // clear the vblank flag
        break ;
    }

    return (value);
}

uint16_t
_load16 (struct cpu * cpu, uint16_t addr)
{
    return ((uint16_t)(_load8 (cpu, addr + 1)) << 8 | _load8 (cpu, addr));
}

void
_store8 (struct cpu * cpu, uint16_t addr, uint8_t param)
{
//    printf ("_store8: %02x@%04x\n", param, addr);

    switch (addr) {
    case 0x2003:
        nes_ppu_spr_ram_set_ptr (cpu, &cpu->ppu, param);                break ;
    case 0x2004:
        nes_ppu_spr_ram_store (cpu, &cpu->ppu, param);                  break ;
    case 0x2005:
        nes_ppu_scroll (cpu, &cpu->ppu, param);                         break ;
    case 0x2006:
        nes_ppu_vram_set_ptr (cpu, &cpu->ppu, param);                   break ;
    case 0x2007:
        nes_ppu_vram_store (cpu, &cpu->ppu, param);                     break ;
    case 0x4014:
        nes_ppu_dma (cpu, &cpu->ppu, param);                            break ;
        break ;
    }

    cpu->mem[addr] = param;

    if (addr < 0x2000) { // RAM mirroring
        addr %= 0x800;

        cpu->mem[addr] = param;
        cpu->mem[addr + 0x800] = param;
        cpu->mem[addr + 0x1000] = param;
        cpu->mem[addr + 0x1800] = param;
    }
}

unsigned int
_call_brk (struct cpu * cpu, uint8_t op, uint16_t param)
{
    cpu->regs.new_pc += 2;
    cpu->regs.b = 1;

    nes_cpu_interrupt (cpu, INTERRUPT_TYPE_BRK);

    return (0);
}

unsigned int
_call_ora (struct cpu * cpu, uint8_t op, uint16_t param)
{
    cpu->regs.a |= _load8 (cpu, param);

    cpu->regs.z = cpu->regs.a == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.a & 0x80 ? 1 : 0;

    return (0);
}

unsigned int
_call_asl (struct cpu *cpu, uint8_t op, uint16_t param)
{
    if (op == 0x0A) {
        cpu->regs.c = cpu->regs.a & 0x80;
        cpu->regs.a <<= 1;

        cpu->regs.z = cpu->regs.a == 0 ? 1 : 0;
        cpu->regs.n = cpu->regs.a & 0x80 ? 1 : 0;
    }
    else {
        uint8_t     value = _load8 (cpu, param);

        cpu->regs.c = value & 0x80;
        value <<= 1;

        cpu->regs.z = value == 0 ? 1 : 0;
        cpu->regs.n = value & 0x80 ? 1 : 0;

        _store8 (cpu, param, value);
    }

    return (0);
}

unsigned int
_call_php (struct cpu * cpu, uint8_t op, uint16_t param)
{
    cpu->mem[0x100 + cpu->regs.s] = cpu->regs.p;
    cpu->regs.s--;

    return (0);
}

unsigned int
_call_bpl (struct cpu * cpu, uint8_t op, uint16_t param)
{
    if (cpu->regs.n == 0) cpu->regs.new_pc = param;

    return (0);
}

unsigned int
_call_clc (struct cpu * cpu, uint8_t op, uint16_t param)
{
    cpu->regs.c = 0;

    return (0);
}

unsigned int
_call_jsr (struct cpu * cpu, uint8_t op, uint16_t param)
{
    cpu->mem[0x100 + cpu->regs.s]     = cpu->regs.new_pc >> 8;
    cpu->mem[0x100 + cpu->regs.s - 1] = cpu->regs.new_pc & 0x00FF;
    cpu->regs.s -= 2;

    cpu->regs.new_pc = param - 1;

    return (0);
}

unsigned int
_call_and (struct cpu * cpu, uint8_t op, uint16_t param)
{
    cpu->regs.a &= _load8 (cpu, param);

    cpu->regs.z = cpu->regs.a == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.a & 0x80 ? 1 : 0;

    return (0);
}

unsigned int
_call_bit (struct cpu * cpu, uint8_t op, uint16_t param)
{
    uint8_t value = _load8 (cpu, param);

    cpu->regs.z = (value & cpu->regs.a) == 0 ? 1 : 0;
    cpu->regs.n = value & 0x80 ? 1 : 0;
    cpu->regs.v = value & 0x40 ? 1 : 0;

    return (0);
}

unsigned int
_call_rol (struct cpu * cpu, uint8_t op, uint16_t param)
{
    if (op == 0x2A) {
        cpu->regs.c = cpu->regs.a & 0x80;
        cpu->regs.a <<= 1;
        cpu->regs.a |= cpu->regs.c;

        cpu->regs.z = cpu->regs.a == 0 ? 1 : 0;
        cpu->regs.n = cpu->regs.a & 0x80 ? 1 : 0;
    }
    else {
        uint8_t     value = _load8 (cpu, param);

        cpu->regs.c = value & 0x80;
        value <<= 1;
        value |= cpu->regs.c;

        cpu->regs.z = value == 0 ? 1 : 0;
        cpu->regs.n = value & 0x80 ? 1 : 0;

        _store8 (cpu, param, value);
    }

    return (0);
}

unsigned int
_call_plp (struct cpu * cpu, uint8_t op, uint16_t param)
{
    cpu->regs.p = cpu->mem[0x100 + cpu->regs.s + 1];
    cpu->regs.s++;

    return (0);
}

unsigned int
_call_bmi (struct cpu * cpu, uint8_t op, uint16_t param)
{
    if (cpu->regs.n == 1) cpu->regs.new_pc = param;

    return (0);
}


unsigned int
_call_sec (struct cpu * cpu, uint8_t op, uint16_t param)
{
    cpu->regs.c = 1;

    return (0);
}


unsigned int
_call_rti (struct cpu * cpu, uint8_t op, uint16_t param)
{
    cpu->regs.p = cpu->mem[0x100 + cpu->regs.s + 1];
    cpu->regs.new_pc = cpu->mem[0x100 + cpu->regs.s + 2];
    cpu->regs.new_pc |= (uint16_t)(cpu->mem[0x100 + cpu->regs.s + 3]) << 8;
    cpu->regs.s += 3;

    return (0);
}

unsigned int
_call_eor (struct cpu * cpu, uint8_t op, uint16_t param)
{
    cpu->regs.a ^= _load8 (cpu, param);

    cpu->regs.z = cpu->regs.a == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.a & 0x80 ? 1 : 0;

    return (0);
}

unsigned int
_call_lsr (struct cpu * cpu, uint8_t op, uint16_t param)
{
    if (op == 0x4A) {
        cpu->regs.c = cpu->regs.a & 0x80;
        cpu->regs.a >>= 1;

        cpu->regs.z = cpu->regs.a == 0 ? 1 : 0;
        cpu->regs.n = cpu->regs.a & 0x80 ? 1 : 0;
    }
    else {
        uint8_t     value = _load8 (cpu, param);

        cpu->regs.c = value & 0x80;
        value >>= 1;

        cpu->regs.z = value == 0 ? 1 : 0;
        cpu->regs.n = value & 0x80 ? 1 : 0;

        _store8 (cpu, param, value);
    }

    return (0);
}

unsigned int
_call_pha (struct cpu * cpu, uint8_t op, uint16_t param)
{
    cpu->mem[0x100 + cpu->regs.s] = cpu->regs.a;
    cpu->regs.s--;

    return (0);
}

unsigned int
_call_jmp (struct cpu * cpu, uint8_t op, uint16_t param)
{
    cpu->regs.new_pc = param - 1;

    return (0);
}

unsigned int
_call_bvc (struct cpu * cpu, uint8_t op, uint16_t param)
{
    if (cpu->regs.v == 0) cpu->regs.new_pc = param;

    return (0);
}

unsigned int
_call_cli (struct cpu * cpu, uint8_t op, uint16_t param)
{
    cpu->regs.i = 0;

    return (0);
}

unsigned int
_call_rts (struct cpu * cpu, uint8_t op, uint16_t param)
{
    cpu->regs.new_pc = cpu->mem[0x100 + cpu->regs.s + 1];
    cpu->regs.new_pc |= (uint16_t)(cpu->mem[0x100 + cpu->regs.s + 2]) << 8;
    cpu->regs.s += 2;

    return (0);
}

unsigned int
_call_adc (struct cpu * cpu, uint8_t op, uint16_t param)
{
    uint32_t    temp;

    temp = cpu->regs.a + _load8 (cpu, param) + cpu->regs.c;
    cpu->regs.v = (!(((cpu->regs.a ^ _load8 (cpu, param))) & 0x80) != 0) &&
        (((cpu->regs.a ^ temp) & 0x80)) != 0 ? 1 : 0;

    cpu->regs.c = temp > 255 ? 1 : 0;
    cpu->regs.z = temp == 0 ? 1 : 0;
    cpu->regs.n = temp & 0x80 ? 1 : 0;

    cpu->regs.a = temp;

    return (0);
}

unsigned int
_call_ror (struct cpu * cpu, uint8_t op, uint16_t param)
{
    if (op == 0x6A) {
        cpu->regs.c = cpu->regs.a & 0x01;
        cpu->regs.a >>= 1;
        cpu->regs.a |= (cpu->regs.c << 7);

        cpu->regs.z = cpu->regs.a == 0 ? 1 : 0;
        cpu->regs.n = cpu->regs.a & 0x80 ? 1 : 0;
    }
    else {
        uint8_t     value = _load8 (cpu, param);

        cpu->regs.c = value & 0x01;
        value >>= 1;
        value |= (cpu->regs.c << 7);

        cpu->regs.z = value == 0 ? 1 : 0;
        cpu->regs.n = value & 0x80 ? 1 : 0;

        _store8 (cpu, param, value);
    }

    return (0);
}

unsigned int
_call_pla (struct cpu * cpu, uint8_t op, uint16_t param)
{
    cpu->regs.a = cpu->mem[0x100 + cpu->regs.s + 1];
    cpu->regs.s++;

    return (0);
}

unsigned int
_call_bvs (struct cpu * cpu, uint8_t op, uint16_t param)
{
    if (cpu->regs.n == 1) cpu->regs.new_pc = param;

    return (0);
}

unsigned int
_call_sei (struct cpu * cpu, uint8_t op, uint16_t param)
{
    cpu->regs.i = 1;

    return (0);
}

unsigned int
_call_sta (struct cpu * cpu, uint8_t op, uint16_t param)
{
    _store8 (cpu, param, cpu->regs.a);

    return (0);
}

unsigned int
_call_stx (struct cpu * cpu, uint8_t op, uint16_t param)
{
    _store8 (cpu, param, cpu->regs.x);

    return (0);
}

unsigned int
_call_sty (struct cpu * cpu, uint8_t op, uint16_t param)
{
    _store8 (cpu, param, cpu->regs.y);

    return (0);
}

unsigned int
_call_dey (struct cpu * cpu, uint8_t op, uint16_t param)
{
    cpu->regs.y--;

    cpu->regs.z = cpu->regs.y == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.y & 0x80 ? 1 : 0;

    return (0);
}

unsigned int
_call_txa (struct cpu * cpu, uint8_t op, uint16_t param)
{
    cpu->regs.a = cpu->regs.x;

    cpu->regs.z = cpu->regs.a == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.a & 0x80 ? 1 : 0;

    return (0);
}

unsigned int
_call_tya (struct cpu * cpu, uint8_t op, uint16_t param)
{
    cpu->regs.a = cpu->regs.y;

    cpu->regs.z = cpu->regs.a == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.a & 0x80 ? 1 : 0;

    return (0);
}

unsigned int
_call_bcc (struct cpu * cpu, uint8_t op, uint16_t param)
{
    if (cpu->regs.c == 0) cpu->regs.new_pc = param;

    return (0);
}

unsigned int
_call_txs (struct cpu * cpu, uint8_t op, uint16_t param)
{
    cpu->regs.s = cpu->regs.x;

    return (0);
}

unsigned int
_call_lda (struct cpu * cpu, uint8_t op, uint16_t param)
{
    cpu->regs.a = _load8 (cpu, param);

    cpu->regs.z = cpu->regs.a == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.a & 0x80 ? 1 : 0;

    return (0);
}

unsigned int
_call_ldx (struct cpu * cpu, uint8_t op, uint16_t param)
{
    cpu->regs.x = _load8 (cpu, param);

    cpu->regs.z = cpu->regs.x == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.x & 0x80 ? 1 : 0;

    return (0);
}

unsigned int
_call_ldy (struct cpu * cpu, uint8_t op, uint16_t param)
{
    cpu->regs.y = _load8 (cpu, param);

    cpu->regs.z = cpu->regs.y == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.y & 0x80 ? 1 : 0;

    return (0);
}

unsigned int
_call_tay (struct cpu * cpu, uint8_t op, uint16_t param)
{
    cpu->regs.y = cpu->regs.a;

    cpu->regs.z = cpu->regs.y == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.y & 0x80 ? 1 : 0;

    return (0);
}

unsigned int
_call_tax (struct cpu * cpu, uint8_t op, uint16_t param)
{
    cpu->regs.x = cpu->regs.a;

    cpu->regs.z = cpu->regs.x == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.x & 0x80 ? 1 : 0;

    return (0);
}

unsigned int
_call_bcs (struct cpu * cpu, uint8_t op, uint16_t param)
{
    if (cpu->regs.c == 1) cpu->regs.new_pc = param;

    return (0);
}

unsigned int
_call_clv (struct cpu * cpu, uint8_t op, uint16_t param)
{
    cpu->regs.v = 0;

    return (0);
}

unsigned int
_call_tsx (struct cpu * cpu, uint8_t op, uint16_t param)
{
    cpu->regs.x = cpu->regs.s;

    return (0);
}

unsigned int
_call_cpy (struct cpu * cpu, uint8_t op, uint16_t param)
{
    int result = cpu->regs.x - _load8 (cpu, param);

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

    return (0);
}

unsigned int
_call_cmp (struct cpu * cpu, uint8_t op, uint16_t param)
{
    int result = cpu->regs.a - _load8 (cpu, param);

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

    return (0);
}

unsigned int
_call_dec (struct cpu * cpu, uint8_t op, uint16_t param)
{
    uint8_t     value = _load8 (cpu, param);

    value -= 1;

    cpu->regs.z = value == 0 ? 1 : 0;
    cpu->regs.n = value & 0x80 ? 1 : 0;

    _store8 (cpu, param, value);

    return (0);
}

unsigned int
_call_iny (struct cpu * cpu, uint8_t op, uint16_t param)
{
    cpu->regs.y++;

    cpu->regs.z = cpu->regs.y == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.y & 0x80 ? 1 : 0;

    return (0);
}

unsigned int
_call_dex (struct cpu * cpu, uint8_t op, uint16_t param)
{
    cpu->regs.x--;

    cpu->regs.z = cpu->regs.x == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.x & 0x80 ? 1 : 0;

    return (0);
}

unsigned int
_call_bne (struct cpu * cpu, uint8_t op, uint16_t param)
{
    if (cpu->regs.z == 0) cpu->regs.new_pc = param;

    return (0);
}

unsigned int
_call_cld (struct cpu * cpu, uint8_t op, uint16_t param)
{
    cpu->regs.d = 0;

    return (0);
}

unsigned int
_call_cpx (struct cpu * cpu, uint8_t op, uint16_t param)
{
    int result = cpu->regs.x - _load8 (cpu, param);

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

    return (0);
}

unsigned int
_call_sbc (struct cpu * cpu, uint8_t op, uint16_t param)
{
    int32_t     temp;
    uint8_t     value = _load8 (cpu, param);

    temp = cpu->regs.a - value - (1 - cpu->regs.c);

    cpu->regs.v = (((cpu->regs.a ^ temp) & 0x80) != 0 &&
                   ((cpu->regs.a ^ value) & 0x80) != 0) ? 1 : 0;
    cpu->regs.c = temp < 0 ? 1 : 0;
    cpu->regs.z = temp == 0 ? 1 : 0;
    cpu->regs.n = temp & 0x80 ? 1 : 0;

    return (0);
}

unsigned int
_call_inc (struct cpu * cpu, uint8_t op, uint16_t param)
{
    uint8_t     value = _load8 (cpu, param);

    value += 1;

    cpu->regs.z = value == 0 ? 1 : 0;
    cpu->regs.n = value & 0x80 ? 1 : 0;

    _store8 (cpu, param, value);

    return (0);
}

unsigned int
_call_inx (struct cpu * cpu, uint8_t op, uint16_t param)
{
    cpu->regs.x++;

    cpu->regs.z = cpu->regs.x == 0 ? 1 : 0;
    cpu->regs.n = cpu->regs.x & 0x80 ? 1 : 0;

    return (0);
}

unsigned int
_call_nop (struct cpu * cpu, uint8_t op, uint16_t param)
{
    return (0);
}

unsigned int
_call_beq (struct cpu * cpu, uint8_t op, uint16_t param)
{
    if (cpu->regs.z == 1) cpu->regs.new_pc = param;

    return (0);
}

unsigned int
_call_sed (struct cpu * cpu, uint8_t op, uint16_t param)
{
    cpu->regs.d = 1;

    return (0);
}

unsigned int
_call_inv (struct cpu * cpu, uint8_t op, uint16_t param)
{
    return (0);
}

uint16_t
_extract_param (struct cpu *    cpu,
                uint8_t         addr_mode)
{
    switch (addr_mode) {
    case ADDR_MODE_ZPA:  // zero page mode
        return (cpu->mem[cpu->regs.pc + 2]);

    case ADDR_MODE_REL:  // relative mode
        return (cpu->regs.new_pc + (int8_t)cpu->mem[cpu->regs.pc + 2]);

    case ADDR_MODE_ABS:  // absolute
        return (((uint16_t)(cpu->mem[cpu->regs.pc + 3]) << 8) |
                cpu->mem[cpu->regs.pc + 2]);

    case ADDR_MODE_ACC:  // accumulator
        return (cpu->regs.a);

    case ADDR_MODE_IMM:  // immediate mode
        return (cpu->regs.pc + 2);

    case ADDR_MODE_ZPX:  // zero page, x
        return (cpu->mem[cpu->regs.pc + 2] + cpu->regs.x);

    case ADDR_MODE_ZPY:  // zero page, y
        return (cpu->mem[cpu->regs.pc + 2] + cpu->regs.y);

    case ADDR_MODE_ABX:  // absolute, x
        return (((uint16_t)(cpu->mem[cpu->regs.pc + 3]) << 8 |
                 cpu->mem[cpu->regs.pc + 2]) + cpu->regs.x);

    case ADDR_MODE_ABY:  // absolute, y
        return (((uint16_t)(cpu->mem[cpu->regs.pc + 3]) << 8 |
                 cpu->mem[cpu->regs.pc + 2]) + cpu->regs.y);

    case ADDR_MODE_INX: // indirect, x (indexed indirect)
        return (_load16 (cpu, cpu->mem[cpu->regs.pc + 2] + cpu->regs.x));

    case ADDR_MODE_INY: // indirect, y (indirect indexed)
        return (_load16 (cpu, cpu->mem[cpu->regs.pc + 2]) + cpu->regs.y);

    case ADDR_MODE_IAB: // indirect absolute
        return (_load16 (cpu,
                         (uint16_t)(cpu->mem[cpu->regs.pc + 3]) << 8 |
                         cpu->mem[cpu->regs.pc + 2]));

    }
    return (0);
}

void
nes_cpu_init (struct nes * nes,
              struct cpu * cpu)
{
    nes_ppu_init (nes, cpu, &cpu->ppu);

    memset (cpu->mem, 0x00, sizeof (cpu->mem));
    memset (&cpu->mem[0x2001], 0x00, sizeof (cpu->mem) - 0x2000);

    if (nes->header.prg_rom_size == 1)
        memcpy (&cpu->mem[0xC000], nes->prg_rom, 16384);
    else
        memcpy (&cpu->mem[0x8000], nes->prg_rom, nes->header.prg_rom_size * 16384);

    cpu->regs.a = 0;
    cpu->regs.x = 0;
    cpu->regs.y = 0;
    cpu->regs.s = 0xFF;
    cpu->regs.p = 0x00;

    cpu->debug.run = 0;
    cpu->debug.count = 1;

    nes_cpu_interrupt (cpu, INTERRUPT_TYPE_RST);
    cpu->regs.pc = cpu->regs.new_pc;
}

void
nes_cpu_interrupt (struct cpu *         cpu,
                   enum interrupt_type  interrupt_type)
{
    uint16_t    addr;

    cpu->mem[0x100 + cpu->regs.s]     = cpu->regs.new_pc >> 8;
    cpu->mem[0x100 + cpu->regs.s - 1] = cpu->regs.new_pc & 0x00FF;
    cpu->mem[0x100 + cpu->regs.s - 2] = cpu->regs.p;
    cpu->regs.s -= 3;

    switch (interrupt_type) {
    case INTERRUPT_TYPE_NMI:
        addr = _load16 (cpu, 0xFFFA);
        break ;
    case INTERRUPT_TYPE_RST:
        addr = _load16 (cpu, 0xFFFC);
        break ;
    case INTERRUPT_TYPE_BRK:
        if (!cpu->regs.i)
            addr = _load16 (cpu, 0xFFFE);
        break ;
    default:
        addr = 0;
    }
    cpu->regs.new_pc = addr - 1;

//    printf ("INTERRUPT! %d -> %04x\n", interrupt_type, cpu->regs.new_pc);
}

int
nes_cpu_disassemble (struct nes * nes,
                     struct cpu * cpu)
{
    uint16_t            pc;
    uint8_t             op;

    pc = nes->header.prg_rom_size == 1 ? 0xC000 : 0x8000;
    while (pc < 0xFFFF) {
        op = cpu->mem[pc];

        printf ("[%04x][%02x]\t %s",
                pc, (unsigned char)op, opcodes[op].name);
        if (opcodes[op].len == 2) {
            printf ("(%02x)  ", cpu->mem[pc + 1]);
        } else if (opcodes[op].len == 3) {
            printf ("(%04x)",
                    (uint16_t)(cpu->mem[pc + 2]) << 8 | cpu->mem[pc + 1]);
        } else if (!strcmp(opcodes[op].name, "INV")) {
            printf ("ALID  ");
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

        if (opcodes[op].addr_mode == ADDR_MODE_REL) {
            if (!strcmp (opcodes[op].name, "BCC")) {
                printf("   C=0");
            }
            else if (!strcmp (opcodes[op].name, "BCS")) {
                printf("   C=1");
            }
            else if (!strcmp (opcodes[op].name, "BEQ")) {
                printf("   Z=1");
            }
            else if (!strcmp (opcodes[op].name, "BNE")) {
                printf("   Z=0");
            }
            else if (!strcmp (opcodes[op].name, "BMI")) {
                printf("   N=1");
            }
            else if (!strcmp (opcodes[op].name, "BPL")) {
                printf("   N=0");
            }
            else if (!strcmp (opcodes[op].name, "BVS")) {
                printf("   V=1");
            }
            else if (!strcmp (opcodes[op].name, "BVC")) {
                printf("   V=0");
            }
            printf (" => %04x", pc + (int8_t)cpu->mem[pc + 1]);
        }
        printf ("\n");

        pc += opcodes[op].len;
    }
    return (0);
}

int
nes_cpu_exec (struct nes * nes,
              struct cpu * cpu,
              uint32_t     options)
{
    uint8_t             op;
    uint16_t            param;

    unsigned int        cycles;

    op = cpu->mem[cpu->regs.pc + 1];

    cpu->regs.new_pc += opcodes[op].len;

    param = _extract_param (cpu, opcodes[op].addr_mode);

    cycles = opcodes[op].time + opcodes[op].call (cpu, op, param);

    if (cpu->debug.run > 0) {
        cpu->debug.run -= cycles;
    }
    if (options & NES_DEBUG && cpu->debug.run <= 0) {
        printf ("\x1b[32m[%04x>%04x]\x1b[0m[%02x]\t \x1b[31m%s\x1b[0m",
                cpu->regs.pc + 1, cpu->regs.new_pc + 1, (unsigned char)op, opcodes[op].name);
        if (opcodes[op].len == 2) {
            printf ("(\x1b[34m%02x\x1b[0m)  ", cpu->mem[cpu->regs.pc + 2]);
        } else if (opcodes[op].len == 3) {
            printf ("(\x1b[34m%04x\x1b[0m)",
                    (uint16_t)(cpu->mem[cpu->regs.pc + 3]) << 8 | cpu->mem[cpu->regs.pc + 2]);
        } else if (!strcmp(opcodes[op].name, "INV")) {
            printf ("\x1b[31mALID\x1b[0m  ");
        } else {
            printf ("      ");
        }
        printf ("[%04x]", param);


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

        printf("\t\t%dcc - %d\n", cycles, cpu->debug.count);
    }

    cpu->debug.count += cycles;

    for (cycles *= 3; cycles; cycles--)
        nes_ppu_exec (nes, cpu, &cpu->ppu, options);

    cpu->regs.pc = cpu->regs.new_pc & 0xFFFF;
    return (0);
}
