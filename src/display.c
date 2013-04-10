#include <stdio.h>
#include <allegro5/allegro.h>

#include "nes.h"
#include "cpu.h"
#include "display.h"
#include "colors.h"

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
_nes_draw_pixel_for (struct nes *     nes,
                     struct cpu *     cpu,
                     int              ptn_tbl_addr,
                     int              name_table_addr,
                     uint8_t          abs_x,
                     uint8_t          abs_y)
{
    uint8_t     map_x = abs_x / 8;
    uint8_t     map_y = abs_y / 8;

    uint8_t     x = abs_x % 8;
    uint8_t     y = abs_y % 8;

    uint8_t *   pattern_table;
    uint8_t *   name_table;
    uint8_t *   attribute_table;
    uint8_t     tile_addr;
    uint8_t     tile[0x8 * 0x8];

    uint8_t     attribute;
    uint8_t     high_color;

    struct nes_color    color;

    pattern_table =     &cpu->ppu.mem[ptn_tbl_addr * 0x1000];
    name_table =        &cpu->ppu.mem[0x2000 + name_table_addr * 0x400];
    attribute_table =   name_table + 0x3C0;

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

    color = nes_colors[tile[y * 0x8 + x] | (high_color << 2)];
    if (tile[y * 0x8 + x] == 0x24)
        return (0);

    al_draw_pixel (map_x * 8 + x,
                   map_y * 8 + y,
                   al_map_rgb (color.r, color.g, color.b));

    return (0);
}

int
nes_draw_pixel (struct nes *    nes,
                struct cpu *    cpu,
                uint8_t         x,
                uint8_t         y)
{
    _nes_draw_pixel_for (nes, cpu, cpu->ppu.scrn_ptn_tbl_addr, cpu->ppu.name_table_addr, x, y);
    _nes_draw_pixel_for (nes, cpu, cpu->ppu.sprt_ptn_tbl_addr, cpu->ppu.name_table_addr, x, y);

    return (0);
}
