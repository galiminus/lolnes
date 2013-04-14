#include <stdio.h>
#include <allegro5/allegro.h>

#include "nes.h"
#include "display.h"
#include "colors.h"

int _draw_pixel_for (struct nes *, int, int, uint8_t, uint8_t);
int _draw_sprites (struct nes *, int, uint8_t, uint8_t);

int
display_init (struct nes *       nes)
{
    nes->display = al_create_display (256, 240);
    if (nes->display == NULL) {
        fprintf (stderr, "failed to create display!\n");
        return (-1);
    }
    return (0);
}

void
display_destroy (struct nes *    nes)
{
    al_destroy_display (nes->display);
}


int
display_draw (struct nes *    nes,
              uint8_t         x,
              uint8_t         y)
{
    _draw_pixel_for (nes, nes->ppu.sprt_ptn_tbl_addr, nes->ppu.name_table_addr, x, y);
    _draw_pixel_for (nes, nes->ppu.scrn_ptn_tbl_addr, nes->ppu.name_table_addr, x, y);

//    _draw_sprites (nes, nes->ppu.sprt_ptn_tbl_addr, x, y);
    return (0);
}

int
_draw_pixel_for (struct nes *     nes,
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
    uint8_t     tile;

    uint8_t     attribute;
    uint8_t     high_color;

    struct nes_color    color;


    pattern_table =     &nes->ppu.mem[ptn_tbl_addr * 0x1000];
    name_table =        &nes->ppu.mem[0x2000 + name_table_addr * 0x400];
    attribute_table =   name_table + 0x3C0;

    tile_addr = name_table[map_y * 32 + map_x];
    tile = ppu_get_tile (pattern_table, tile_addr * 0x10, x, y);

    attribute = attribute_table[(map_y / 4) * 8 + (map_x / 4)];
    if (!(map_y % 2) && !(map_x % 2))
        high_color = (attribute & 0x03);
    else if (!(map_y % 2) && map_x % 2)
        high_color = (attribute & 0x0C) >> 2;
    else if (map_y % 2 && !(map_x % 2))
        high_color = (attribute & 0x30) >> 4;
    else if (map_y % 2 && map_x % 2)
        high_color = (attribute & 0xC0) >> 6;

    color = nes_colors[tile | (high_color << 2)];
    color = nes_colors[tile];

    al_draw_pixel (abs_x, abs_y, al_map_rgb (color.r, color.g, color.b));
//    if (x == 7 || y == 7)
//        al_draw_pixel (abs_x, abs_y, al_map_rgb (0xff, 0xff, 0xff));
    return (0);
}

int
_draw_sprites (struct nes *    nes,
               int             ptn_tbl_addr,
               uint8_t         abs_x,
               uint8_t         abs_y)
{
    int         i;

    uint8_t     sprt_x;
    uint8_t     sprt_y;

    uint8_t     x = abs_x % 8;
    uint8_t     y = abs_y % 8;

    uint8_t     offset;

    uint8_t *   pattern_table;
    uint8_t     tile;

    uint8_t     high_color;

    struct nes_color    color;

    pattern_table = &nes->ppu.mem[ptn_tbl_addr * 0x1000];
    for (i = 0; i < 0xFF; i += 4) {
        sprt_y = nes->ppu.sprt_mem[i];
        offset = nes->ppu.sprt_mem[i + 1];
        high_color = nes->ppu.sprt_mem[i + 1] & 0x03;
        sprt_x = nes->ppu.sprt_mem[i + 3];

        if ((abs_x >= sprt_x && abs_x < (sprt_x + 0x08)) ||
            (abs_y >= sprt_y && abs_y < (sprt_y + 0x08))) {
            tile = ppu_get_tile (pattern_table, offset * 0x10, x, y);

            color = nes_colors[tile | (high_color << 2)];

            al_draw_pixel (abs_x, abs_y, al_map_rgb (color.r, color.g, color.b));
        }
    }

    return (0);
}
