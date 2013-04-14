#ifndef __DISPLAY_H__
# define __DISPLAY_H__

#include "cpu.h"

int display_init (struct nes *);
void display_destroy (struct nes *);
int display_draw (struct nes *, uint8_t, uint8_t);

#endif /* !__DISPLAY_H__ */
