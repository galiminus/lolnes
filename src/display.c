#include <stdio.h>
#include <allegro5/allegro.h>

#include "nes.h"
#include "cpu.h"
#include "display.h"
#include "colors.h"

int
init_display (struct nes *       nes)
{
//    nes->display = al_create_display (256, 240);
    nes->display = al_create_display (512, 480);
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
    uint8_t *   attribute_table;
    uint8_t     tile_addr;
    uint8_t     tile[0x8 * 0x8];

    uint8_t     attribute;
    uint8_t     high_color;

    struct nes_color    color;

    pattern_table =     &cpu->ppu.mem[cpu->ppu.sprt_ptn_tbl_addr * 0x1000];
    name_table =        &cpu->ppu.mem[0x2000 + cpu->ppu.name_table_addr * 0x400];
    attribute_table =   name_table + 0x3C0;

    for (map_y = 0; map_y < 30; map_y++) {
        for (map_x = 0; map_x < 32; map_x++) {
            tile_addr = name_table[map_y * 32 + map_x];
            nes_ppu_get_tile (pattern_table, tile_addr * 0x10, tile);

            attribute = attribute_table[(map_y / 4) * 8 + (map_x / 4)];
            if (!(map_y % 2) && !(map_x % 2))
                high_color = attribute & 0x03;
            else if (!(map_y % 2) && map_x % 2)
                high_color = attribute & 0x0C >> 2;
            else if (map_y % 2 && !(map_x % 2))
                high_color = attribute & 0x30 >> 4;
            else if (map_y % 2 && map_x % 2)
                high_color = attribute & 0xC0 >> 6;

            for (y = 0; y < 0x8; y++)
                for (x = 0; x < 0x8; x++) {
                    color = nes_colors[tile[y * 0x8 + x] | (high_color << 2)];

                    al_draw_pixel ((map_x * 8 + x) * 2,
                                   (map_y * 8 + y) * 2,
                                   al_map_rgb (color.r, color.g, color.b));
                    al_draw_pixel ((map_x * 8 + x) * 2 + 1,
                                   (map_y * 8 + y) * 2,
                                   al_map_rgb (color.r, color.g, color.b));
                    al_draw_pixel ((map_x * 8 + x) * 2,
                                   (map_y * 8 + y) * 2 + 1,
                                   al_map_rgb (color.r, color.g, color.b));
                    al_draw_pixel ((map_x * 8 + x) * 2 + 1,
                                   (map_y * 8 + y) * 2 + 1,
                                   al_map_rgb (color.r, color.g, color.b));
                }
        }
    }
    al_flip_display ();
    return (0);
}
