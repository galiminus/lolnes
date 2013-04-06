#include <stdio.h>

#include "nes.h"
#include "display.h"

int
main (int argc, const char ** argv)
{
    struct nes nes;

    if (argc < 2) {
        printf ("%s rom\n", argv[0]);
        return (0);
    }

    if (nes_init (&nes) == -1) {
        goto error;
    }
//    if (init_display (&nes) == -1) {
//        goto error;
//    }

    if (nes_open (argv[1], &nes) == -1) {
        goto error;
    }

    nes_exec (&nes, NES_DEBUG);

    return (0);

  error:
    return (-1);
}
