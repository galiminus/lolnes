#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/mman.h>
#include <stdio.h>

#include <string.h>

#include "nes.h"
#include "cpu.h"
#include "ppu.h"
#include "debug.h"
#include "display.h"

int _nes_parse (struct nes *, const char *, size_t);
int _nes_parse_header (struct nes *, const char *, size_t);
int _nes_parse_trainer (struct nes *, const char *, size_t);
int _nes_parse_prg_rom (struct nes *, const char *, size_t);
int _nes_parse_chr_rom (struct nes *, const char *, size_t);

int
nes_init (struct nes *  nes,
          uint32_t      options,
          const char *  path)
{
    struct stat stat;

    if (!al_init ()) {
        fprintf (stderr, "failed to initialize allegro!\n");
        return (-1);
    }
    nes->options = options;

    nes->fd = open (path, O_RDONLY);
    if (nes->fd == -1) {
        perror("open");
        goto error;
    }

    if (fstat (nes->fd, &stat) == -1) {
        perror("fstat");
        goto close_fd;
    }

    nes->rom = mmap (NULL, stat.st_size, PROT_READ, MAP_PRIVATE, nes->fd, 0);
    if (nes->rom == MAP_FAILED) {
        perror("mmap");
        goto close_fd;
    }

    if (_nes_parse (nes, nes->rom, stat.st_size) == -1) {
        goto munmap_rom;
    }

    cpu_init (&nes->cpu, nes);
    ppu_init (&nes->ppu, nes);

    return (0);

  munmap_rom:
    munmap (nes->rom, stat.st_size);
  close_fd:
    close (nes->fd);
  error:
    return (-1);
}


int
nes_exec (struct nes *  nes)
{
    return (cpu_exec (&nes->cpu));
}

int
_nes_parse (struct nes * nes,
            const char * rom,
            size_t       size)
{
    size_t      parsed_size;

    int (*parsers[])(struct nes *, const char *, size_t) = {
        _nes_parse_header,
        _nes_parse_trainer,
        _nes_parse_prg_rom,
        _nes_parse_chr_rom,
        NULL
    };
    unsigned int i;

    for (i = 0; parsers[i]; i++) {
        parsed_size = parsers[i](nes, rom, size);
        if (parsed_size == -1) {
            goto error;
        }
        size -= parsed_size;
        rom += parsed_size;
    }

    return (0);

  error:
    return (-1);
}

int
_nes_parse_header (struct nes * nes,
                   const char * rom,
                   size_t       size)
{
    if (size < 16) {
        printf ("Invalid ROM header size\n");
        return (-1);
    }

    memcpy (nes->header.magic, rom, 4);
    if (memcmp (nes->header.magic, "NES\x1A", 4) != 0) {
        printf ("Invalid ROM header\n");
    }

    nes->header.prg_rom_size = rom[4];
    nes->header.chr_rom_size = rom[5];
    nes->header.flags_6 = rom[6];
    nes->header.flags_7 = rom[7];
    nes->header.pgr_ram_size = rom[8];
    nes->header.flags_9 = rom[9];
    nes->header.flags_10 = rom[10];

    return (16);
}

int
_nes_parse_trainer (struct nes *        nes,
                    const char *        rom,
                    size_t              size)
{
    if (!nes->header.trainer) {
        return (0);
    }
    if (size < 512) {
        printf ("Invalid trainer size\n");
        return (-1);
    }

    nes->trainer = rom;

    return (512);
}

int
_nes_parse_prg_rom (struct nes *        nes,
                    const char *        rom,
                    size_t              size)
{
    size_t      prg_rom_size;

    if (nes->header.prg_rom_size > 2) {
        printf ("> 2k cartridge are not supported\n");
        return (-1);
    }

    prg_rom_size = nes->header.prg_rom_size * 16384;
    if (prg_rom_size < prg_rom_size) {
        printf ("Invalid PRG ROM size\n");
        return (-1);
    }

    nes->prg_rom = rom;

    return (prg_rom_size);
}

int
_nes_parse_chr_rom (struct nes *        nes,
                    const char *        rom,
                    size_t              size)
{
    size_t      chr_rom_size;

    chr_rom_size = nes->header.chr_rom_size * 8192;
    if (size < chr_rom_size) {
        printf ("Invalid CHR ROM size\n");
        return (-1);
    }
    if (chr_rom_size == 0) {
        return (0);
    }

    nes->chr_rom = rom;

    return (chr_rom_size);
}
