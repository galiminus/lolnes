#include <stdio.h>
#include <allegro5/allegro.h>

#include "nes.h"
#include "cpu.h"

int
init_display (struct nes *       nes)
{
    ALLEGRO_BITMAP * bitmap = NULL;

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
    al_draw_pixel (10, 30, al_map_rgb(200, 30, 30));
    al_draw_pixel (10, 31, al_map_rgb(200, 30, 30));
    al_draw_pixel (11, 31, al_map_rgb(200, 30, 30));
    al_draw_pixel (11, 30, al_map_rgb(200, 30, 30));

    al_flip_display ();
    return (0);
}
