#ifndef __CPU_H__
# define __CPU_H__

#include "nes.h"

struct cpu
{
    uint8_t     mem[0x10000];

    struct {
        uint8_t a;

        uint8_t x;
        uint8_t y;

        uint8_t s;

        union {
            struct {
                unsigned short  c       :1; // carry
                unsigned short  z       :1; // zero
                unsigned short  i       :1; // IRQ disabled
                unsigned short  d       :1; // Decimal mode
                unsigned short  b       :1; // Interrupt caused by brk
                unsigned short  _u      :1; // Unused
                unsigned short  v       :1; // Oveflow
                unsigned short  n       :1; // Negative
            };
            uint8_t p;
        };

        uint16_t pc;
        uint16_t new_pc;
    } regs;
};

void nes_cpu_init(struct nes *, struct cpu *);
int nes_cpu_exec(struct nes *, struct cpu *, uint32_t);

#endif /* !__CPU_H__ */
