#ifndef __DISPLAY_H__
# define __DISPLAY_H__

#include "cpu.h"

int init_display (struct nes *);
void destroy_display (struct nes *);
int nes_draw_pixel (struct nes *, struct cpu *, uint8_t, uint8_t);

#endif /* !__DISPLAY_H__ */
