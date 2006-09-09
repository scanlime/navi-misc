/*
 * mi6k.c -- Lock-free MI6K rendering thread based on
 *           PulseAudio's memblockq. This initializes
 *           the hardware and owns queue of rendered
 *           frames.
 *
 * Copyright (C) 2006 Micah Dowty
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <pulsecore/memblock.h>
#include <pulsecore/memblockq.h>
#include "mi6k.h"

struct {
    FILE *device;
    pa_memblockq *queue;
    pa_mempool *pool;
    pthread_t thread;
    pa_memchunk blank_frame;
    int power;
} mi6k;

#define MI6KIO_VFD_POWER  0x3600

/* We run at about 30 frames per second.. so this shuts off the VFD after 10 minutes */
#define AUTO_OFF_FRAMES   (10 * 60 * 30)

#define MI6K_CLEAR        0x15
#define MI6K_HOME         0x16
#define MI6K_CURSOR_OFF   0x0e
#define MI6K_DEFINE_CHAR  0x18
#define MI6K_USER_CHAR(x) (0xF6 + (x))

static void mi6k_define_char(char *pixels, int index)
{
    int bit, byte;
    unsigned char buffer;
    static const int dot_order[8][5] = {
	{ 17, 13,  9,  5,  1 },
	{  0, 31, 27, 23, 19 },
	{ 16, 12,  8,  4,  0 },
	{ 34, 30, 26, 22, 18 },
	{ 15, 11,  7,  3,  0 },
	{ 33, 29, 25, 21,  0 },
	{ 14, 10,  6,  2,  0 },
	{ 32, 28, 24, 20,  0 },
    };

    fputc(MI6K_DEFINE_CHAR, mi6k.device);
    fputc(MI6K_USER_CHAR(index), mi6k.device);

    for (byte=0; byte<5; byte++) {
	buffer = 0;
	for (bit=0; bit<8; bit++) {
	    if (isgraph(pixels[dot_order[bit][byte]]))
		buffer |= 1 << bit;
	}
	fputc(buffer, mi6k.device);
    }
}

void mi6k_init_hardware()
{
    fputc(MI6K_CURSOR_OFF, mi6k.device);
    fputc(MI6K_CLEAR, mi6k.device);

    /* Dim the entire display to the lowest level */
    fwrite("\x19\x30\xff\x07", 1, 4, mi6k.device);

    mi6k_define_char("     "
		     "     "
		     "     "
		     "     "
		     "     "
		     "     "
		     "*****", 0);

    mi6k_define_char("     "
		     "     "
		     "     "
		     "     "
		     "     "
		     "*****"
		     "*****", 1);
    

    mi6k_define_char("     "
		     "     "
		     "     "
		     "     "
		     "*****"
		     "*****"
		     "*****", 2);

    mi6k_define_char("     "
		     "     "
		     "     "
		     "*****"
		     "*****"
		     "*****"
		     "*****", 3);

    mi6k_define_char("     "
		     "     "
		     "*****"
		     "*****"
		     "*****"
		     "*****"
		     "*****", 4);

    mi6k_define_char("     "
		     "*****"
		     "*****"
		     "*****"
		     "*****"
		     "*****"
		     "*****", 5);

    mi6k_define_char("*****"
		     "*****"
		     "*****"
		     "*****"
		     "*****"
		     "*****"
		     "*****", 6);

    fflush(mi6k.device);
}

static void mi6k_set_power(int on)
{
    printf("Setting power: %d\n", on);
    mi6k.power = !!on;
    ioctl(fileno(mi6k.device), MI6KIO_VFD_POWER, mi6k.power);
}

static void *mi6k_render_thread(void *userdata)
{
    while (1) {
	pa_memchunk chunk;
	int i;
	int is_empty = 1;
	static int sleeptimer = 0;

	i = pa_memblockq_peek(mi6k.queue, &chunk);

	/*
	 * Before writing this chunk, let's see if there's anything
	 * printable in it. For our purposes, these include
	 * special characters but not control characters: thus, anything
	 * above the space.
	 */
	for (i=0; i<chunk.length; i++) {
	  int c = ((unsigned char*)chunk.memblock->data)[chunk.index + i];
	  if (c > ' ') {
	    is_empty = 0;
	  }
	}

	if (is_empty) {
	    if (mi6k.power) {
		/*
		 * We're rendering blank frames. Our sleep timer counts down...
		 * If we stay blank for quite a while, power down the VFD.
		 */
		if (++sleeptimer > AUTO_OFF_FRAMES) {
		    mi6k_set_power(0);
		}
	    }
	} else {
	    sleeptimer = 0;

	    if (!mi6k.power) {
		/*
		 * We're rendering non-blank frames, but the display is off.
		 * Turn on the juice, wait for it to start up, then do our initialization.
		 */
		
		mi6k_set_power(1);
		usleep(200000);
		mi6k_init_hardware();
	    }
	}
  	
	fwrite(chunk.memblock->data + chunk.index, 1, chunk.length, mi6k.device);
	fflush(mi6k.device);

	pa_memblock_unref(chunk.memblock);
	pa_memblockq_drop(mi6k.queue, &chunk, chunk.length);
    }
    return NULL;
}

void mi6k_frame_new(pa_memchunk *out, unsigned char **chars)
{
    out->memblock = pa_memblock_new(mi6k.pool, MI6K_WIDTH * MI6K_HEIGHT + 1);
    assert(out->memblock);

    out->index = 0;
    out->length = out->memblock->length;

    memset(out->memblock->data, ' ', out->memblock->length);

    *(unsigned char*)out->memblock->data = MI6K_HOME;
    if (chars) {
	*chars = out->memblock->data + 1;
    }
}

void mi6k_frame_bargraph(pa_memchunk *out, float *columns)
{
    unsigned char *chars;
    int x, y;
    static const unsigned char levels[] = {
	' ', MI6K_USER_CHAR(0), MI6K_USER_CHAR(1), MI6K_USER_CHAR(2),
	MI6K_USER_CHAR(3), MI6K_USER_CHAR(4), MI6K_USER_CHAR(5),
	MI6K_USER_CHAR(6),
    };

    mi6k_frame_new(out, &chars);

    for (x=0; x<MI6K_WIDTH; x++) {
	int level = MI6K_HEIGHT * sizeof levels * columns[x] + 0.5;
	for (y=0; y<MI6K_HEIGHT; y++) {
	    int row = level - y * sizeof levels;
	    if (row < 0)
		row = 0;
	    if (row >= sizeof levels)
		row = sizeof levels - 1;
	    chars[x + (MI6K_HEIGHT - 1 - y) * MI6K_WIDTH] = levels[row];
	}
    }
}

int mi6k_needs_frame()
{
    return pa_memblockq_get_length(mi6k.queue) < MI6K_WIDTH * MI6K_HEIGHT * 2;
}

void mi6k_commit_frame(pa_memchunk *frame)
{
    pa_memblockq_push(mi6k.queue, frame);
    pa_memblock_unref(frame->memblock);
}

void mi6k_init()
{
    int result;

    mi6k.pool = pa_mempool_new(0);

    mi6k.device = fopen("/dev/mi6k0", "wb");
    if (!mi6k.device) {
	perror("Opening MI6K");
	exit(1);
    }

    mi6k_frame_new(&mi6k.blank_frame, NULL);

    mi6k.queue = pa_memblockq_new(0,                              /* Default index */
				  MI6K_WIDTH * MI6K_HEIGHT * 2.5, /* Max length: 2.5 frames */
				  MI6K_WIDTH * MI6K_HEIGHT,       /* Target length: 1 frame */
				  1,                              /* Base value */
				  0,                              /* No prebuffering */
				  1,                              /* Minimal request */
				  mi6k.blank_frame.memblock);     /* "Silence" frame, all blank */
    assert(mi6k.queue);

    result = pthread_create(&mi6k.thread, NULL, mi6k_render_thread, NULL);
    assert(!result);
}
