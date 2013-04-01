#include <stdio.h>

#include "nes.h"

int
main (int argc, const char ** argv)
{
    struct nes nes;

    if (argc < 2) {
        printf ("%s rom\n", argv[0]);
        return (0);
    }

    if (nes_open (argv[1], &nes) == -1) {
        goto error;
    }

    nes_exec (&nes);

    return (0);

  error:
    return (-1);
}
