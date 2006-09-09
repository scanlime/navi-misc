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

void mi6k_frame_new(pa_memchunk *out, unsigned char **chars);
void mi6k_frame_bargraph(pa_memchunk *out, float *columns);
int mi6k_needs_frame(void);
void mi6k_commit_frame(pa_memchunk *frame);
void mi6k_init(void);

#endif /* __MI6K_H__ */
