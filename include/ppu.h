#ifndef __PPU_H__
# define __PPU_H__

#include <time.h>

struct nes;

enum ppu_state {
    PPU_START,
    PPU_BOOT,
    PPU_FRAME_START,
    PPU_DRAW_PIXEL,
    PPU_HBLANK,
    PPU_FRAME_END,
    PPU_VBLANK
};

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

    uint8_t             sprt_mem[0x100];

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
            unsigned short      _unknown2               :4;
            unsigned short      read_only               :1;
            unsigned short      sprt_per_line           :1;
            unsigned short      hit                     :1;
            unsigned short      vblank                  :1;
        };
        uint8_t s_regs;
    };
    unsigned short      name_mirroring  :2;

    uint16_t            vram_ptr;

    enum ppu_state      state;

    uint8_t             x;
    uint8_t             y;

    uint32_t            boot_counter;
    uint32_t            hblank_counter;
    uint32_t            vblank_counter;

    struct nes *        nes;
};

struct color_layout {

};

#define BOOT_DELAY      30000
#define HBLANK_DELAY    86
#define VBLANK_DELAY    6820

#define MIRROR_ALL              0x00
#define MIRROR_VERTICAL         0x01
#define MIRROR_HORIZONTAL       0x02
#define MIRROR_FOUR_SCREEN      0x03

void ppu_init (struct ppu *, struct nes *);
void ppu_exec (struct ppu *);

void ppu_dma (struct ppu *, uint8_t);
void ppu_vblank_interrupt (struct ppu *);
void ppu_spr_ram_set_ptr (struct ppu *, uint8_t);
void ppu_spr_ram_store (struct ppu *, uint8_t);
void ppu_vram_set_ptr (struct ppu *, uint8_t);
void ppu_vram_store (struct ppu *, uint8_t);
void ppu_scroll (struct ppu *, uint8_t);
uint8_t ppu_get_tile (const uint8_t *, uint16_t, uint8_t, uint8_t);

#endif /* !__PPU_H__ */
