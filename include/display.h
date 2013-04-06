#ifndef __DISPLAY_H__
# define __DISPLAY_H__

#include "cpu.h"

int init_display (struct nes *);
int destroy_display (struct nes *);
int nes_display (struct nes *, struct cpu *);

#endif /* !__DISPLAY_H__ */
