#include "nes.h"
#include "cpu.h"

#include "ppu.h"

void
nes_ppu_init (struct nes * nes,
              struct cpu * cpu,
              struct ppu * ppu)
{
    cpu->mem[0x2002] = 0xF0;
    return ;
}

void
nes_ppu_exec (struct nes *      nes,
              struct cpu *      cpu,
              struct ppu *      ppu,
              uint32_t          options)
{
    ppu->c_regs_1 = cpu->mem[0x2000];
    ppu->c_regs_2 = cpu->mem[0x2001];
    ppu->s_regs = cpu->mem[0x2002];
    ppu->sprt_memory_addr = cpu->mem[0x2003];
    ppu->sprt_memory_data = cpu->mem[0x2004];
    ppu->scrn_scroll_offsets = cpu->mem[0x2005];
    ppu->ppu_memory_addr = cpu->mem[0x2006];
    ppu->ppu_memory_data = cpu->mem[0x2007];

    return ;
}
