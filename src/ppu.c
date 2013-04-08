#include <time.h>
#include <stdio.h>

#include <assert.h>

#include "nes.h"
#include "cpu.h"
#include "display.h"

#include "ppu.h"

void
nes_ppu_init (struct nes * nes,
              struct cpu * cpu,
              struct ppu * ppu)
{
    memset (ppu->mem, 0, sizeof (ppu->mem));
    memcpy (ppu->mem, nes->chr_rom, nes->header.chr_rom_size * 0x1000);

    ppu->vram_ptr = 0;
    ppu->name_mirroring = nes->header.a11 << 1 | nes->header.a10;
    ppu->next_frame = 32;
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

    ppu->next_frame--;
    if (ppu->next_frame == 0) {
        if (cpu->mem[0x2000] & 0x80) {
            printf ("DISPLAY\n");
            nes_display (nes, cpu);
        }


//        nes_ppu_vblank_interrupt (cpu, ppu);
//        nes_display (nes, cpu);
        ppu->vblank = 1;
    } else if (ppu->next_frame == -87833) {
        ppu->read_only = 1;
        ppu->vblank = 0;
        ppu->next_frame = FRAME_DELAY;
    }

    cpu->mem[0x2000] = ppu->c_regs_1;
    cpu->mem[0x2001] = ppu->c_regs_2;
    cpu->mem[0x2002] = ppu->s_regs;

    return ;
}

void
nes_ppu_dma (struct cpu *       cpu,
             struct ppu *       ppu,
             uint8_t            value)
{
    memcpy (ppu->sprt_mem, &cpu->mem[value], sizeof (ppu->sprt_mem));
}

void
nes_ppu_spr_ram_set_ptr (struct cpu *  cpu,
                         struct ppu *  ppu,
                         uint8_t       value)
{
    cpu->mem[0x2004] = ppu->sprt_mem[value];
}

void
nes_ppu_spr_ram_store (struct cpu *  cpu,
                       struct ppu *  ppu,
                       uint8_t       value)
{
    ppu->sprt_mem[cpu->mem[0x2003]] = value;
    nes_ppu_spr_ram_set_ptr (cpu, ppu, cpu->mem[0x2003] + 1);
}

void
nes_ppu_vram_set_ptr (struct cpu *      cpu,
                      struct ppu *      ppu,
                      uint8_t           value)
{
    ppu->vram_ptr = (ppu->vram_ptr << 8) | value;
    cpu->mem[0x2007] = ppu->mem[ppu->vram_ptr];
}

void
nes_ppu_vram_store (struct cpu *  cpu,
                    struct ppu *  ppu,
                    uint8_t       value)
{
    if (!ppu->read_only) {
        if (ppu->vram_ptr >= 0x2000 && ppu->vram_ptr < 0x3000) {
            uint16_t name_offset = (ppu->vram_ptr - 0x2000) % 0x400;

            switch (ppu->name_mirroring) {
            case MIRROR_ALL:
                ppu->mem[0x2000 + name_offset] = value;
                ppu->mem[0x2400 + name_offset] = value;
                ppu->mem[0x2800 + name_offset] = value;
                ppu->mem[0x2C00 + name_offset] = value;
                break ;
            case MIRROR_VERTICAL:
                if ((ppu->vram_ptr >= 0x2000 && ppu->vram_ptr < 0x2400) ||
                    (ppu->vram_ptr >= 0x2800 && ppu->vram_ptr < 0x2C00)) {
                    ppu->mem[0x2000 + name_offset] = value;
                    ppu->mem[0x2800 + name_offset] = value;
                }
                else if ((ppu->vram_ptr >= 0x2400 && ppu->vram_ptr < 0x2800) ||
                         (ppu->vram_ptr >= 0x2C00 && ppu->vram_ptr < 0x3000)) {
                    ppu->mem[0x2400 + name_offset] = value;
                    ppu->mem[0x2C00 + name_offset] = value;
                }
                break ;
            case MIRROR_HORIZONTAL:
                if (ppu->vram_ptr >= 0x2000 && ppu->vram_ptr < 0x2800) {
                    ppu->mem[0x2000 + name_offset] = value;
                    ppu->mem[0x2400 + name_offset] = value;
                }
                else if (ppu->vram_ptr >= 0x2800 && ppu->vram_ptr < 0x3000) {
                    ppu->mem[0x2800 + name_offset] = value;
                    ppu->mem[0x2C00 + name_offset] = value;
                }
                break ;
            case MIRROR_FOUR_SCREEN:
                ppu->mem[ppu->vram_ptr] = value;
                break ;
            }
        } else {
            ppu->mem[ppu->vram_ptr] = value;
        }
    }
    if (ppu->vertical_write) {
        ppu->vram_ptr += 32;
        cpu->mem[0x2007] += 32;
    } else {
        ppu->vram_ptr += 1;
        cpu->mem[0x2007] += 1;
    }
}

void
nes_ppu_scroll (struct cpu *    cpu,
                struct ppu *    ppu,
                uint8_t         param)
{
    return ;
}

void
nes_ppu_vblank_interrupt (struct cpu *       cpu,
                          struct ppu *       ppu)
{
    ppu->vblank = 1;
    ppu->vblank_enable = 0;

    nes_cpu_interrupt (cpu, INTERRUPT_TYPE_NMI);

    ppu->next_frame = FRAME_DELAY;
}

void
nes_ppu_get_tile (const uint8_t *       graph_mem,
                  uint16_t              addr,
                  uint8_t *             tile)
{
    int y;
    int x;

    for (y = 0; y < 0x8; y++)
        for (x = 0; x < 0x8; x++)
            tile[y * 0x08 + x] =
                ((graph_mem[addr + y] & (0x80 >> x)) >> (7 - x))                |
                ((graph_mem[addr + y + 0x08] & (0x80 >> x)) >> (7 - x)) << 1;
}

