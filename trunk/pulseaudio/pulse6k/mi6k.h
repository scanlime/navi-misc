/*
 * mi6k.h -- Lock-free MI6K rendering thread based on
 *           PulseAudio's memblockq. This initializes
 *           the hardware and owns queue of rendered
 *           frames.
 *
 * Copyright (C) 2006 Micah Dowty
 */

#ifndef __MI6K_H__
#define __MI6K_H__

#include <pulsecore/memchunk.h>

#define MI6K_WIDTH        20
#define MI6K_HEIGHT       2

#define MI6K_BRIGHTNESS_MIN    0x07
#define MI6K_BRIGHTNESS_MAX    0x00

#define MI6K_BRIGHTNESS(x) ((int) (MI6K_BRIGHTNESS_MIN + \
                           (MI6K_BRIGHTNESS_MAX - MI6K_BRIGHTNESS_MIN) * (x) + 0.5))

struct mi6k_dim_command {
  unsigned char cmd[2];
  unsigned char column;
  unsigned char level;
};

void mi6k_frame_new(pa_memchunk *out,
		    unsigned char **chars_out,
		    struct mi6k_dim_command **dim_command_out);
void mi6k_frame_bargraph(pa_memchunk *out, float *columns);
int mi6k_needs_frame(void);
void mi6k_commit_frame(pa_memchunk *frame);
void mi6k_init(void);

#endif /* __MI6K_H__ */
