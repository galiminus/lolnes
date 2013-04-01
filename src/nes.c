#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/mman.h>
#include <stdio.h>

#include <string.h>

#include "nes.h"
#include "cpu.h"

int _nes_parse_header (const char *, size_t, struct nes *);
int _nes_parse_trainer (const char *, size_t, struct nes *);
int _nes_parse_prg_rom (const char *, size_t, struct nes *);
int _nes_parse_chr_rom (const char *, size_t, struct nes *);

int
nes_open (const char *  path,
          struct nes *  nes)
{
    struct stat stat;

    char *      rom;

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

    if (nes_parse (nes->rom, stat.st_size, nes) == -1) {
        goto munmap_rom;
    }

    return (0);

  munmap_rom:
    munmap (nes->rom, stat.st_size);
  close_fd:
    close (nes->fd);
  error:
    return (-1);
}

int
nes_parse (const char * rom,
           size_t       size,
           struct nes * nes)
{
    size_t      parsed_size;

    int (*parsers[])(const char *, size_t, struct nes *) = {
        _nes_parse_header,
        _nes_parse_trainer,
        _nes_parse_prg_rom,
        _nes_parse_chr_rom,
        NULL
    };
    unsigned int i;

    for (i = 0; parsers[i]; i++) {
        parsed_size = parsers[i](rom, size, nes);
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
_nes_parse_header (const char * rom,
                   size_t       size,
                   struct nes * nes)
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
_nes_parse_trainer (const char *        rom,
                    size_t              size,
                    struct nes *        nes)
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
_nes_parse_prg_rom (const char *        rom,
                    size_t              size,
                    struct nes *        nes)
{
    size_t      prg_rom_size;

    prg_rom_size = nes->header.prg_rom_size * 16384;
    if (prg_rom_size < prg_rom_size) {
        printf ("Invalid PRG ROM size\n");
        return (-1);
    }

    nes->prg_rom = rom;

    return (prg_rom_size);
}

int
_nes_parse_chr_rom (const char *        rom,
                    size_t              size,
                    struct nes *        nes)
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

nes_exec (struct nes * nes)
{
    struct cpu  cpu;

    nes_cpu_init (nes, &cpu);
    for (;;) {
        nes_cpu_exec (nes, &cpu);
    }
}
