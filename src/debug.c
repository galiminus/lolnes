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
        else {
            printf ("Unknown command: %s\n", action);
        }
    }
    return (0);
}
