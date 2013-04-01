#ifndef __NES_H__
# define __NES_H__

#include <stdint.h>
#include <stdlib.h>

struct nes
{
    char *      rom;
    int         fd;

    struct {
        char    magic[4];
        uint8_t prg_rom_size;
        uint8_t chr_rom_size;
        union {
            /*
              76543210
              ||||||||
              ||||+||+- 0xx0: vertical arran  gement/horizontal mirroring (CIRAM A10 = PPU A11)
              |||| ||   0xx1: horizontal arrangement/vertical mirroring (CIRAM A10 = PPU A10)
              |||| ||   1xxx: four-screen VRAM
              |||| |+-- 1: SRAM in CPU $6000-$7FFF, if present, is battery backed
              |||| +--- 1: 512-byte trainer at $7000-$71FF (stored before PRG data)
              ++++----- Lower nybble of mapper number
             */
            struct {
                unsigned short  vh                  :1;
                unsigned short  sram_in_cpu         :1;
                unsigned short  trainer             :1;
                unsigned short  four_screen_vram    :1;
                unsigned short  lower_mapper_number :4;
            };
            uint8_t flags_6;
        };

        union {
            /*
              76543210
              ||||||||
              |||||||+- VS Unisystem
              ||||||+-- PlayChoice-10 (8KB of Hint Screen data stored after CHR data)
              ||||++--- If equal to 2, flags 8-15 are in NES 2.0 format
              ++++----- Upper nybble of mapper number
            */
            struct {
                unsigned short  vs_unisystem            :1;
                unsigned short  playchoice              :1;
                unsigned short  flags_8_15_format       :2;
                unsigned short  upper_mapper_number     :4;
            };
            uint8_t flags_7;
        };

        uint8_t pgr_ram_size;

        union {
            /*
              76543210
              ||||||||
              |||||||+- TV system (0: NTSC; 1: PAL)
              +++++++-- Reserved, set to zero
            */
            struct {
                unsigned short tv_system_1      :1;
                unsigned short _reserved        :7;
            };
            uint8_t flags_9;
        };

        union {
            /*
              76543210
              ||  ||
              ||  ++- TV system (0: NTSC; 2: PAL; 1/3: dual compatible)
              |+----- SRAM in CPU $6000-$7FFF is 0: present; 1: not present
              +------ 0: Board has no bus conflicts; 1: Board has bus conflicts
            */
            struct {
                unsigned short tv_system_2              :2;
                unsigned short _unused                  :2;
                unsigned short sram_in_cpu2             :1;
                unsigned short board_has_bus_conflicts  :1;
                unsigned short _unused_2                :2;
            };
            uint8_t flags_10;
        };
    } header;

    const char *        trainer;
    const char *        prg_rom;
    const char *        chr_rom;
};

#define NES_DEBUG 0x01

int nes_open (const char *, struct nes *);
int nes_parse (const char *, size_t, struct nes *);
int nes_exec (struct nes *, uint32_t);

#endif /* !__NES_H__ */
