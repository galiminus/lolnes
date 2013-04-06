#include <stdio.h>
#include <allegro5/allegro.h>

#include "nes.h"
#include "cpu.h"

int
init_display (struct nes *       nes)
{
    nes->display = al_create_display (256, 240);
    if (nes->display == NULL) {
        fprintf (stderr, "failed to create display!\n");
        return (-1);
    }
    return (0);
}

void
destroy_display (struct nes *    nes)
{
    al_destroy_display (nes->display);
}

int
nes_display (struct nes *       nes,
             struct cpu *       cpu)
{
    int         map_y;
    int         map_x;

    int         y;
    int         x;

    uint8_t *   pattern_table;
    uint8_t *   name_table;
    uint8_t     tile_addr;
    uint8_t     tile[0x8 * 0x8];

    pattern_table = &cpu->ppu.mem[cpu->ppu.sprt_ptn_tbl_addr * 0x1000];
    name_table = &cpu->ppu.mem[0x2000 + cpu->ppu.name_table_addr * 0x400];
    for (map_y = 0; map_y < 30; map_y++) {
        for (map_x = 0; map_x < 32; map_x++) {
            tile_addr = name_table[map_y * 32 + map_x];

            nes_ppu_get_tile (pattern_table, tile_addr * 0x10, tile);

            for (y = 0; y < 0x8; y++)
                for (x = 0; x < 0x8; x++) {
                    al_draw_pixel (map_x * 8 + x,
                                   map_y * 8 + y,
                                   al_map_rgb (tile[y * 0x8 + x] * 30, 30, 30));
                }
        }
    }
    al_flip_display ();
    return (0);
}
