#include <stdio.h>
#include <string.h>

#include "nes.h"
#include "cpu.h"
#include "ppu.h"

void
_nes_put_memory (const uint8_t * memory,
                 unsigned int    offset,
                 unsigned int    size)
{
    unsigned int        i;

    printf ("%04x: ", offset);
    for (i = 0; i < size; i++) {
        printf("%02x ", (unsigned char)memory[offset + i]);
        if (!((i + 1) % 32) && (i + 1) < size) {
            printf ("\n%04x: ", offset + i);
        }
    }
    printf("\n");
}

void
_nes_put_stack (const uint8_t * memory,
                unsigned int    offset,
                unsigned int    size)
{
    unsigned int        i;

    printf ("%04x: ", offset);
    for (i = size; i; i--) {
        printf("%02x ", (unsigned char)memory[offset - i]);
        if (i - 1)
            printf ("\n%04x: ", offset - i - 0x100);
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
        else if (!strcmp (action, "ppumem")) {
            size_t      size;
            size_t      offset;

            if (!strcmp (argv[0], "pattern1")) {
                offset = 0x000;
                size = 0x1000;
            } else if (!strcmp (argv[0], "pattern2")) {
                offset = 0x1000;
                size = 0x1000;
            } else if (!strcmp (argv[0], "name1")) {
                offset = 0x2000;
                size = 0x3C0;
            } else if (!strcmp (argv[0], "attr1")) {
                offset = 0x23C0;
                size = 0x40;
            } else if (!strcmp (argv[0], "name2")) {
                offset = 0x2400;
                size = 0x3C0;
            } else if (!strcmp (argv[0], "attr2")) {
                offset = 0x27C0;
                size = 0x40;
            } else if (!strcmp (argv[0], "name3")) {
                offset = 0x2800;
                size = 0x3C0;
            } else if (!strcmp (argv[0], "attr3")) {
                offset = 0x2BC0;
                size = 0x40;
            } else if (!strcmp (argv[0], "name4")) {
                offset = 0x2C00;
                size = 0x3C0;
            } else if (!strcmp (argv[0], "attr4")) {
                offset = 0x2FC0;
                size = 0x40;
            } else if (!strcmp (argv[0], "unused")) {
                offset = 0x3000;
                size = 0xEFF;
            } else if (!strcmp (argv[0], "pcolors")) {
                offset = 0x3F00;
                size = 0x10;
            } else if (!strcmp (argv[0], "pcolors")) {
                offset = 0x3F10;
                size = 0x10;
            } else if (!strcmp (argv[0], "colorsmirrors")) {
                offset = 0x3F20;
                size = 0xE0;
            } else {
                offset = argc > 0 ? strtoul(argv[0], NULL, 16) : 0x0000;
                size = argc > 1 ? strtoul(argv[1], NULL, 16) : 0x2000;
            }

            _nes_put_memory (cpu->ppu.mem, offset, size);
        }
        else if (!strcmp (action, "stack")) {
            _nes_put_stack (cpu->mem, 0x200, 0xFF - cpu->regs.s);
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
            printf ("master_save_mode: %d\n", ppu->master_slave_mode);
            printf ("vblank_enable: %d\n", ppu->vblank_enable);
            printf ("img_mask: %d\n", ppu->img_mask);
            printf ("sprt_mask: %d\n", ppu->sprt_mask);
            printf ("scrn_enable: %d\n", ppu->scrn_enable);
            printf ("sprt_enable: %d\n", ppu->sprt_enable);
            printf ("background_color: %01x\n", ppu->background_color);
            printf ("hit: %d\n", ppu->hit);
            printf ("vblank: %d\n", ppu->vblank);
        }
        else {
            printf ("Unknown command: %s\n", action);
        }
    }
    return (0);
}
