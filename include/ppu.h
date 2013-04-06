#ifndef __PPU_H__
# define __PPU_H__

#include <time.h>

#include "cpu.h"

struct ppu
{
    union {
        struct {
            uint8_t     pattern_table1[0x1000];
            uint8_t     pattern_table2[0x1000];

            uint8_t     name_table1[0x3C0];
            uint8_t     attribute_table1[0x3C0];

            uint8_t     name_table2[0x3C0];
            uint8_t     attribute_table2[0x3C0];

            uint8_t     name_table3[0x3C0];
            uint8_t     attribute_table3[0x3C0];

            uint8_t     name_table4[0x3C0];
            uint8_t     attribute_table4[0x3C0];

            uint8_t     _unused_table[0xEFF];

            uint8_t     picture_colors[0x10];
            uint8_t     sprite_colors[0x10];

            uint8_t     _colors_mirrors[0xE0];
        };
        uint8_t         mem[0x4000];
    };

    union {
        struct {
            unsigned short      name_table_addr         :2;
            unsigned short      vertical_write          :1;
            unsigned short      sprt_ptn_tbl_addr       :1;
            unsigned short      scrn_ptn_tbl_addr       :1;
            unsigned short      sprt_size               :1;
            unsigned short      master_slave_mode       :1; // Not used in NES
            unsigned short      vblank_enable           :1;
        };
        uint8_t c_regs_1;
    };
    union {
        struct {
            unsigned short      _unknown1               :1;
            unsigned short      img_mask                :1;
            unsigned short      sprt_mask               :1;
            unsigned short      scrn_enable             :1;
            unsigned short      sprt_enable             :1;
            unsigned short      background_color        :3;
        };
        uint8_t c_regs_2;
    };

    union {
        struct {
            unsigned short      _unknown2               :6;
            unsigned short      hit                     :1;
            unsigned short      vblank                  :1;
        };
        uint8_t s_regs;
    };
    uint8_t             sprt_memory_addr;
    uint8_t             sprt_memory_data;
    uint8_t             scrn_scroll_offsets;
    uint8_t             ppu_memory_addr;
    uint8_t             ppu_memory_data;

    uint32_t            next_frame;
};

#define FRAME_DELAY     29557

void nes_ppu_init (struct nes *, struct cpu *, struct ppu *);
void nes_ppu_exec (struct nes *, struct cpu *, struct ppu *, uint32_t);

#endif /* !__PPU_H__ */
