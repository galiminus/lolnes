#include <stdio.h>
#include <string.h>

#include "nes.h"
#include "cpu.h"
#include "ppu.h"

void
_nes_put_memory (const char *   memory,
                 size_t         offset,
                 size_t         size)
{
    size_t      i;

    printf ("%04x: ", offset);
    for (i = 0; i < size; i++) {
        printf("%02x ", (unsigned char)memory[offset + i]);
        if (!((i + 1) % 32) && (i + 1) < size) {
            printf ("\n%04x: ", offset + i);
        }
    }
    printf("\n");
}

int
nes_cmd (struct nes *   nes,
         struct cpu *   cpu,
         struct ppu *   ppu)
{
    char                cmd[256];
    char *              action;
    char *              saveptr;

    char *              argv[16];
    unsigned int        argc;

    if (cpu->debug.run > 0) {
        return (0);
    }

    for (;;) {
        printf (">>> ");
        fgets (cmd, sizeof (cmd), stdin);

        action = strtok_r (cmd,  " \n", &saveptr);
        for (argc = 0; argc < sizeof (argv); argc++)
            if (!(argv[argc] = strtok_r (NULL, " \n", &saveptr))) break ;

        if (action == NULL || !strcmp (action, "next")) {
            printf("\033[F");
            break ;
        }
        else if (!strcmp (action, "exit") || !strcmp (action, "quit")) {
            return (-1);
        }
        else if (!strcmp (action, "mem")) {
            size_t      size;
            size_t      offset;

            offset = argc > 0 ? strtoul(argv[0], NULL, 16) : 0x0000;
            size = argc > 1 ? strtoul(argv[1], NULL, 16) : 0x2000;

            _nes_put_memory (cpu->mem, offset, size);
        }
        else if (!strcmp (action, "stack")) {
            _nes_put_memory (cpu->mem, 0x100, 0x100);
        }
        else if (!strcmp (action, "checkpoint")) {
            if (argc < 1) {
                printf ("offset required\n");
                continue ;
            }
            cpu->debug.checkpoint = strtoul(argv[0], NULL, 16);
            printf ("checkpoint set to %04x\n", cpu->debug.checkpoint);
        }
        else if (!strcmp (action, "run")) {
            if (argc < 1) {
                printf ("number of instructions required\n");
                continue ;
            }
            cpu->debug.run = strtoul(argv[0], NULL, 10);
            printf ("run for %d instructions\n", cpu->debug.run);
            break ;
        }
        else if (!strcmp (action, "ppuinfo")) {
            printf ("name_table_addr: %01x\n", ppu->name_table_addr);
            printf ("vertical_write: %d\n", ppu->vertical_write);
            printf ("sprt_ptn_tbl_addr: %d\n", ppu->sprt_ptn_tbl_addr);
            printf ("scrn_ptn_tbl_addr: %d\n", ppu->scrn_ptn_tbl_addr);
            printf ("sprt_size: %d\n", ppu->sprt_size);
            printf ("master_save_mode: \n", ppu->master_slave_mode);
            printf ("vblank_enable: %d\n", ppu->vblank_enable);
            printf ("img_mask: %d\n", ppu->img_mask);
            printf ("sprt_mask: %d\n", ppu->sprt_mask);
            printf ("scrn_enable: %d\n", ppu->scrn_enable);
            printf ("sprt_enable: %d\n", ppu->sprt_enable);
            printf ("background_color: %01x\n", ppu->background_color);
            printf ("hit: %d\n", ppu->hit);
            printf ("vblank: %d\n", ppu->vblank);

            printf ("sprt_memory_addr: %01x\n", ppu->sprt_memory_addr);
            printf ("sprt_memory_data: %01x\n", ppu->sprt_memory_data);
            printf ("scrn_scroll_offsets: %01x\n", ppu->scrn_scroll_offsets);
            printf ("ppu_memory_addr: %01x\n", ppu->ppu_memory_addr);
            printf ("ppu_memory_data: %01x\n", ppu->ppu_memory_data);
        }
        else {
            printf ("Unknown command: %s\n", action);
        }
    }
    return (0);
}
