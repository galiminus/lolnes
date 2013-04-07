#include <stdio.h>

#include "nes.h"
#include "display.h"

int
main (int argc, const char ** argv)
{
    struct nes  nes;
    uint32_t    options = NES_DEBUG | NES_DISASSEMBLE;

    if (argc < 2) {
        printf ("%s rom\n", argv[0]);
        return (0);
    }

    if (nes_init (&nes) == -1) {
        goto error;
    }
    if (!(options & NES_DISASSEMBLE) && init_display (&nes) == -1) {
        goto error;
    }

    if (nes_open (argv[1], &nes) == -1) {
        goto error;
    }

    nes_exec (&nes, options);

    return (0);

  error:
    return (-1);
}
