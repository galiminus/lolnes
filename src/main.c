#include <stdio.h>

#include "nes.h"
#include "display.h"
#include "debug.h"

int
main (int argc, const char ** argv)
{
    struct nes  nes;
    uint32_t    options = NES_DEBUG;

    if (argc < 2) {
        printf ("%s rom\n", argv[0]);
        return (0);
    }

    if (nes_init (&nes, options, argv[1]) == -1) {
        goto error;
    }
    if (!(nes.options & NES_DISASSEMBLE) && display_init (&nes) == -1) {
        goto error;
    }

    for (;;) {
        if (options & NES_DEBUG && debug_cmd (&nes) == -1) {
            return (-1);
        }
        nes_exec (&nes);
    }

    return (0);

  error:
    return (-1);
}
