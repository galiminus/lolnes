#include <time.h>
#include <stdio.h>

#include <assert.h>

#include "nes.h"
#include "cpu.h"
#include "display.h"

#include "ppu.h"

void
ppu_init (struct ppu * ppu,
          struct nes * nes)
{
    memset (ppu->mem, 0, sizeof (ppu->mem));
    memcpy (ppu->mem, nes->chr_rom, nes->header.chr_rom_size * 0x2000);

    ppu->vram_ptr = 0;
    ppu->name_mirroring = nes->header.a11 << 1 | nes->header.a10;

    ppu->state = PPU_START;

    return ;
}

void
ppu_exec (struct ppu *      ppu)
{
    ppu->c_regs_1 = ppu->nes->cpu.mem[0x2000];
    ppu->c_regs_2 = ppu->nes->cpu.mem[0x2001];
    ppu->s_regs = ppu->nes->cpu.mem[0x2002];

    switch (ppu->state) {
    case PPU_START:
        ppu->boot_counter = BOOT_DELAY;
        ppu->state = PPU_BOOT;
        ppu->vblank = 1;
        break ;

    case PPU_BOOT:
        ppu->boot_counter--;
        if (ppu->boot_counter == 0) {
            ppu->state = PPU_FRAME_START;
        }
        break ;

    case PPU_FRAME_START:
        ppu->x = 0;
        ppu->y = 0;
        ppu->state = PPU_DRAW_PIXEL;
        break ;

    case PPU_DRAW_PIXEL:
        display_draw (ppu->nes, ppu->x, ppu->y);
        if (ppu->x == 255) {
            if (ppu->y == 242) {
                al_flip_display ();

                if (!ppu->vblank_enable) {
                    ppu->state = PPU_FRAME_START;
h                    break ;
                }

                ppu->vblank = 1;
                ppu->hit = 0;
                ppu_vblank_interrupt (ppu);

                ppu->vblank_counter = VBLANK_DELAY;
                ppu->state = PPU_VBLANK;
            }
            else {
                ppu->hblank_counter = HBLANK_DELAY;
                ppu->state = PPU_HBLANK;
            }
        }
        else {
            ppu->x += 1;
        }
        break ;

    case PPU_HBLANK:
        ppu->hblank_counter--;
        if (ppu->hblank_counter == 0) {
            ppu->x = 0;
            ppu->y += 1;
            ppu->state = PPU_DRAW_PIXEL;
        }
        break ;

    case PPU_VBLANK:
        ppu->vblank_counter--;
        if (ppu->vblank_counter == 0) {
            ppu->vblank = 0;
            ppu->state = PPU_FRAME_END;
        }
        break ;

    case PPU_FRAME_END:
        ppu->state = PPU_FRAME_START;
        break ;
    }

    ppu->nes->cpu.mem[0x2000] = ppu->c_regs_1;
    ppu->nes->cpu.mem[0x2001] = ppu->c_regs_2;
    ppu->nes->cpu.mem[0x2002] = ppu->s_regs;

    return ;
}

void
ppu_dma (struct ppu *       ppu,
         uint8_t            value)
{
    memcpy (ppu->sprt_mem, &ppu->nes->cpu.mem[value], sizeof (ppu->sprt_mem));
}

void
ppu_spr_ram_set_ptr (struct ppu *  ppu,
                     uint8_t       value)
{
    ppu->nes->cpu.mem[0x2004] = ppu->sprt_mem[value];
}

void
ppu_spr_ram_store (struct ppu *  ppu,
                   uint8_t       value)
{
    ppu->sprt_mem[ppu->nes->cpu.mem[0x2003]] = value;
    ppu_spr_ram_set_ptr (ppu, ppu->nes->cpu.mem[0x2003] + 1);
}

void
ppu_vram_set_ptr (struct ppu *      ppu,
                  uint8_t           value)
{
    ppu->vram_ptr = (ppu->vram_ptr << 8) | value;
    ppu->nes->cpu.mem[0x2007] = ppu->mem[ppu->vram_ptr % 0x4000];
}

void
ppu_vram_store (struct ppu *  ppu,
                uint8_t       value)
{
    ppu->vram_ptr %= 0x4000;

    if (ppu->vram_ptr >= 0x3000 && ppu->vram_ptr < 0x3F00) // Mirroring
        ppu->vram_ptr -= 0x1000;

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
        if (ppu->vram_ptr < 0x2F00) // Mirroring
            ppu->mem[ppu->vram_ptr + 0x1000] = value;

    } else {
        ppu->mem[ppu->vram_ptr] = value;
    }

    if (ppu->vertical_write) {
        ppu->vram_ptr += 32;
        ppu->nes->cpu.mem[0x2006] += 32;
    } else {
        ppu->vram_ptr += 1;
        ppu->nes->cpu.mem[0x2006] += 1;
    }
}

void
ppu_scroll (struct ppu *    ppu,
            uint8_t         param)
{
    return ;
}

void
ppu_vblank_interrupt (struct ppu *       ppu)
{
    ppu->vblank = 1;
    ppu->vblank_enable = 0;

    cpu_interrupt (&ppu->nes->cpu, INTERRUPT_TYPE_NMI);
}

uint8_t
ppu_get_tile (const uint8_t *       graph_mem,
              uint16_t              addr,
              uint8_t               x,
              uint8_t               y)
{
    return (((graph_mem[addr + y] & (0x80 >> x)) >> (7 - x)) |
            ((graph_mem[addr + y + 0x08] & (0x80 >> x)) >> (7 - x)) << 1);
}

