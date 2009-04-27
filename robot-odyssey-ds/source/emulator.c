/* -*- Mode: C; c-basic-offset: 4 -*-
 *
 * Hardware emulation and environment support code for sbt86, an
 * experimental 8086 -> C static binary translator.
 *
 * Copyright (c) 2009 Micah Dowty <micah@navi.cx>
 *
 *    Permission is hereby granted, free of charge, to any person
 *    obtaining a copy of this software and associated documentation
 *    files (the "Software"), to deal in the Software without
 *    restriction, including without limitation the rights to use,
 *    copy, modify, merge, publish, distribute, sublicense, and/or sell
 *    copies of the Software, and to permit persons to whom the
 *    Software is furnished to do so, subject to the following
 *    conditions:
 *
 *    The above copyright notice and this permission notice shall be
 *    included in all copies or substantial portions of the Software.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *    OTHER DEALINGS IN THE SOFTWARE.
 */

#include "sbt86.h"

#include <nds.h>
#include <gbfs.h>
#include <stdio.h>

extern const GBFS_FILE data_gbfs;

/*
 * XXX: Use less memory. For now, this is just set to the maximum
 * amount that real-mode code can address.
 */
uint8_t mem[0xFFFF0 + 0xFFFF];
jmp_buf dosExitJump;

static struct {
    int eventWaiting;
    int scancode;
    int ascii;
} keyboard;

/*
 * We emulate the PC speaker in a way which is very specific to how
 * Robot Odyssey uses it. It doesn't use the PIT at all, it just turns
 * the speaker on and off programmatically, relying on cycle timing to
 * generate the right noises. We rely on the cycle counter that we
 * insert into the binary translated code in order to timestamp all
 * I/O operations. When the speaker is toggled, we store a timestamp
 * into a buffer. This buffer is then converted to audio by our
 * audioCallback().
 *
 * XXX: We don't have a good way to slow down the CPU when it gets
 *      too far ahead of the audio thread, so currently the audio
 *      buffer must be large enough to store the longest sound
 *      effect. (The transporter, I think.)
 */

#define AUDIO_BUFFER_SIZE 0x10000  // Must be a power of two
#define AUDIO_HZ          11000    // Seems to sound best at 11kHz. (low-pass filtering)
#define PC_CLOCK_HZ       4770000  // 4.77 MHz

static struct {
    uint8_t       port61;

    struct {
        int       enable;
        uint32_t  currentTime;
        uint8_t   state;
    } playback;

    volatile struct {
        uint32_t  timestamps[AUDIO_BUFFER_SIZE];
        uint32_t  head;
        uint32_t  tail;
    } buffer;
} audio;

int bg;

static uint16_t keyboardPoll(void);


/*****************************************************************
 * Utilitity functions that are referenced by generated code
 */

void
decompressRLE(uint8_t *dest, uint8_t *src, uint32_t srcLength)
{
    /*
     * Decompress our very simple RLE format. Runs of 2 or more zeroes
     * are replaced with 2 zeroes plus a 16-bit count of the omitted
     * zeroes. We assume the output buffer has already been
     * zero-filled, so the zero runs are simply skipped.
     */

    int zeroes = 0;

    while (srcLength) {
        uint8_t byte = *(src++);
        *(dest++) = byte;
        srcLength--;

        if (byte) {
            zeroes = 0;
        } else {
            zeroes++;

            if (zeroes == 2) {
                zeroes = 0;
                dest += src[0] + (src[1] << 8);
                src += 2;
                srcLength -= 2;
            }
        }
    }
}

void
failedDynamicBranch(uint16_t cs, uint16_t ip, uint32_t value)
{
    printf("ERROR: Dynamic branch at %04X:%04X to unsupported value 0x%04x\n",
           cs, ip, value);
    exit(1);
}


/*****************************************************************
 * Emulated DOS, BIOS, and PC hardware.
 */

Regs
interrupt10(Regs reg)
{
    switch (reg.ah) {

    case 0x00: {              /* Set video mode */
        /* Ignore. We're always in CGA mode. */
        break;
    }

    default: {
        printf("BIOS10: Unsupported! ax=0x%04x\n", reg.ax);
        break;
    }
    }
    return reg;
}

Regs
interrupt16(Regs reg)
{
    switch (reg.ah) {

    case 0x00: {              /* Get keystroke */
        reg.ax = keyboardPoll();
        break;
    }

    case 0x01: {              /* Check for keystroke */
        uint16_t key = keyboardPoll();
        if (key) {
            CLR_ZF;
            reg.ax = key;
        } else {
            SET_ZF;
        }
        break;
    }

    default: {
        printf("BIOS16: Unsupported! ax=0x%04x\n", reg.ax);
        break;
    }
    }
    return reg;
}

Regs
interrupt21(Regs reg)
{
    static int numFiles = 0;
    static struct {
        const uint8_t *data;
        uint32_t len;
    } files[16];

    switch (reg.ah) {

    case 0x06: {              /* Direct console input/output (Only input supported) */
        if (reg.dl == 0xFF) {
            uint16_t key = keyboardPoll();
            if (key) {
                reg.al = (uint8_t) key;
                CLR_ZF;
            } else {
                reg.al = 0;
                SET_ZF;
            }
        }
        break;
    }

    case 0x25: {              /* Set interrupt vector */
        /* Ignored. Robot Odyssey uses this to set the INT 24h error handler. */
        break;
    }

    case 0x2C: {              /* Get system time */
        /*
         * XXX: This is supposed to return wallclock time, but Robot Odyssey's
         *      menu just uses this for calculated delays- so the offset from
         *      real time is not important.
         */
        uint32_t ticks = 0; // XXX NDS
        uint32_t seconds = ticks / 1000;
        uint32_t minutes = seconds / 60;
        uint32_t hours = minutes / 60;
        reg.ch = hours % 24;
        reg.cl = minutes % 60;
        reg.dh = seconds % 60;
        reg.dl = (ticks / 10) % 100;
        break;
    }


    case 0x3D: {              /* Open File */
        int fd = numFiles;
        const char *name = (char*)(mem + SEG(reg.ds, reg.dx));

        printf("DOS: Open file %04x:%04x='%s' -> #%d\n",
               reg.ds, reg.dx, name, fd);

        numFiles++;
        files[fd].data = gbfs_get_obj(&data_gbfs, name, &files[fd].len);
        reg.ax = fd;
        CLR_CF;

        if (!files[fd].data) {
            printf("Error opening file");
            exit(1);
        }
        break;
    }

    case 0x3E: {              /* Close File */
        printf("DOS: Close file #%d\n", reg.bx);
        CLR_CF;
        break;
    }

    case 0x3F: {              /* Read File */
        int fd = reg.bx;
        int len = reg.cx;
        void *dest = mem + SEG(reg.ds, reg.dx);

        if (len > files[fd].len) {
            len = files[fd].len;
        }
        memcpy(dest, files[fd].data, len);
        files[fd].data += len;
        files[fd].len -= len;
        reg.ax = len;

        printf("DOS: Read %d bytes from file #%d -> %d bytes at %04x:%04x\n",
               reg.cx, fd, reg.ax, reg.ds, reg.dx);
        CLR_CF;
        break;
    }

    case 0x4C: {              /* Exit with return code */
        longjmp(dosExitJump, 0x100 | reg.al);
        break;
    }

    default:
        printf("DOS: Unsupported! ax=0x%04x\n", reg.ax);
        break;
    }
    return reg;
}

uint8_t
in(uint16_t port, uint32_t timestamp)
{
    switch (port) {

    case 0x61:    /* PC speaker gate */
        return audio.port61;

    default:
        printf("IO: Unimplemented IN %0x04x\n", port);
        return 0;
    }
}

void
out(uint16_t port, uint8_t value, uint32_t timestamp)
{
    switch (port) {

    case 0x43:    /* PIT mode bits */
        /*
         * Ignored. We don't emulate the PIT, we just assume the
         * speaker is always being toggled manually.
         */
        break;

    case 0x61:    /* PC speaker gate */
        if ((value ^ audio.port61) & 2) {
            /*
             * PC speaker state toggled. Store a timestamp.
             */

            uint32_t nextHead = (audio.buffer.head + 1) & (AUDIO_BUFFER_SIZE - 1);

            if (nextHead == audio.buffer.tail) {
                printf("AUDIO: Buffer overflow!\n");
            }

            audio.buffer.timestamps[audio.buffer.head] = timestamp;
            audio.buffer.head = nextHead;

            /*
             * If the audio wasn't playing, start it.
             */
            if (!audio.playback.enable) {
                audio.playback.currentTime = timestamp;
                audio.playback.enable = 1;
                // XXX NDS
            }
        }
        audio.port61 = value;
        break;

    default:
        printf("IO: Unimplemented OUT 0x%04x, %02x\n", port, value);
    }
}


/*****************************************************************
 * Main Program
 */

extern int tutorial_main(const char *cmdLine);

int
main(int argc, char **argv)
{
    int retval;

    videoSetMode(MODE_5_2D);
    vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
    consoleDemoInit();
    bg = bgInit(2, BgType_Bmp16, BgSize_B16_256x256, 0,0);

    iprintf("Hello World!\n");

    retval = tutorial_main("21");

    printf("DOS Exit (return code %d)\n", retval);
    return retval;
}


/*****************************************************************
 * Console
 */

static void
cgaDrawPlane(uint8_t *in, uint16_t *out)
{
    static const uint16_t palette[4] = {
        RGB8(0x00, 0x00, 0x00) | 0x8000,
        RGB8(0x55, 0xff, 0xff) | 0x8000,
        RGB8(0xff, 0x55, 0xff) | 0x8000,
        RGB8(0xff, 0xff, 0xff) | 0x8000,
    };

    int x, y;

    for (y = 96; y; y--) {
        uint16_t *line = out;

        /*
         * Draw one byte (4 CGA pixels) at a time.
         *
         * XXX: Only drawing left side of screen for now.
         */

        for (x = 64; x; x--) {
            uint8_t byte = *(in++);

            line[3] = palette[byte & 3];
            byte >>= 2;
            line[2] = palette[byte & 3];
            byte >>= 2;
            line[1] = palette[byte & 3];
            byte >>= 2;
            line[0] = palette[byte & 3];

            line += 4;
        }

        in += 80 - 64;
        out += 256 * 2;
    }
}

static uint16_t
keyboardPoll(void)
{
    int i;
    static const struct {
        int      key;
        uint16_t code;
    } keyTable[] = {

        { KEY_UP,    0x4800 },
        { KEY_DOWN,  0x5000 },
        { KEY_LEFT,  0x4B00 },
        { KEY_RIGHT, 0x4D00 },

        { KEY_UP    | KEY_R,  0x4800 | '8' },
        { KEY_DOWN  | KEY_R,  0x5000 | '2' },
        { KEY_LEFT  | KEY_R,  0x4B00 | '4' },
        { KEY_RIGHT | KEY_R,  0x4D00 | '6' },

        { KEY_B, ' ' },
        { KEY_A, 'S' },
        { KEY_X, 'C' },
        { KEY_Y, 'H' },
        { KEY_L, 'T' },
    };

    scanKeys();

    for (i = 0; i < (sizeof keyTable / sizeof keyTable[0]); i++) {
        if ((keysHeld() & keyTable[i].key) == keyTable[i].key) {
            return keyTable[i].code;
        }
    }

    return 0;
}

void
consoleBlitToScreen(uint8_t *fb)
{
    int i;

    i = 5;
    while (i--)
        swiWaitForVBlank();

    cgaDrawPlane(fb, bgGetGfxPtr(bg));
    cgaDrawPlane(fb + 0x2000, bgGetGfxPtr(bg) + 256);
}

#if 0
void
audioCallback(void *userdata, uint8_t *buffer, int len)
{
    int sample;

    for (sample = 0; sample < len; sample++) {
        if (audio.buffer.head == audio.buffer.tail) {
            /* Buffer empty, stop playback. */
            audio.playback.enable = 0;
            // XXX NDS
            break;
        }

        /*
         * Advance the audio clock, measuring its progress in CPU cycles.
         */
        audio.playback.currentTime += PC_CLOCK_HZ / AUDIO_HZ;

        /*
         * Slurp up any events from the timestamp buffer which have
         * elapsed by now, and adjust the current speaker state
         * accordingly.
         */
        while (audio.buffer.head != audio.buffer.tail &&
               audio.buffer.timestamps[audio.buffer.tail] < audio.playback.currentTime) {
            audio.buffer.tail = (audio.buffer.tail + 1) & (AUDIO_BUFFER_SIZE - 1);
            audio.playback.state ^= 0xFF;
        }

        buffer[sample] = audio.playback.state;
    }
}
#endif
