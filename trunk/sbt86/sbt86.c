/* -*- Mode: C; c-basic-offset: 4 -*-
 *
 * Environment support code for sbt86, an
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
#include <SDL.h>

/*
 * XXX: Use less memory. For now, this is just set to the maximum
 * amount that real-mode code can address.
 */
uint8_t mem[0xFFFF0 + 0xFFFF];
jmp_buf dosExitJump;

static SDL_Surface *screen;

static struct {
    int eventWaiting;
    int scancode;
    int ascii;
} keyboard;

static void consoleInit();
static void consolePollEvents();


/*****************************************************************
 * Emulated DOS, BIOS, and PC hardware.
 */

Regs
int10(Regs reg)
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
int16(Regs reg)
{
    consolePollEvents();

    switch (reg.ah) {

    case 0x00: {              /* Get keystroke */
        if (keyboard.eventWaiting) {
            reg.ah = keyboard.scancode;
            reg.al = keyboard.ascii;
            printf("BIOS16: Got key %04x\n", reg.ax);
            keyboard.eventWaiting = 0;
        } else {
            reg.ax = 0;
        }
        break;
    }

    case 0x01: {              /* Check for keystroke */
        if (keyboard.eventWaiting) {
            CLR_ZF;
            reg.ah = keyboard.scancode;
            reg.al = keyboard.ascii;
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
int21(Regs reg)
{
    static int numFiles = 0;
    static FILE *files[16];

    consolePollEvents();

    switch (reg.ah) {

    case 0x06: {              /* Direct console input/output (Only input supported) */
        if (reg.dl == 0xFF) {
            if (keyboard.eventWaiting) {
                reg.al = keyboard.ascii;
                CLR_ZF;
                keyboard.eventWaiting = 0;
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
        uint32_t ticks = SDL_GetTicks();
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
        const char *name = mem + SEG(reg.ds, reg.dx);

        printf("DOS: Open file %04x:%04x='%s' -> #%d\n",
               reg.ds, reg.dx, name, fd);

        numFiles++;
        files[fd] = fopen(name, "rb");
        reg.ax = fd;
        CLR_CF;

        if (!files[fd]) {
            perror("fopen");
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
        int result = fread(dest, 1, len, files[fd]);

        printf("DOS: Read %d bytes from file #%d -> %d bytes at %04x:%04x\n",
               len, fd, result, reg.ds, reg.dx);
        reg.ax = result;
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

Regs
int3(Regs reg)
{
    printf("INT 3\n");
    return reg;
}

uint8_t
in(uint16_t port)
{
    printf("IN %04x\n", port);
    return 0;
}

void
out(uint16_t port, uint8_t value)
{
    printf("OUT %04x, %02x\n", port, value);
}


/*****************************************************************
 * Main Program
 */

int
main(int argc, char **argv)
{
    const char *cmdLine = argc > 1 ? argv[1] : "";
    int retval;

    consoleInit();

    retval = game_main(cmdLine);

    printf("DOS Exit (return code %d)\n", retval);
    return retval;
}


/*****************************************************************
 * SDL Console
 */

static void
consoleInit()
{
    if (SDL_Init(SDL_INIT_VIDEO) == -1) {
        fprintf(stderr, "SDL initialization error: %s\n", SDL_GetError());
        exit(1);
    }

    screen = SDL_SetVideoMode(640, 400, 32, 0);
    if (!screen) {
        fprintf(stderr, "Error setting video mode: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_WM_SetCaption("SBT86", "sbt86");
    SDL_EnableKeyRepeat(200, 10);
}

static int
consoleThread(void *arg)
{
    while (1) {
        consolePoll();

    }

    return 0;
}

static void
cgaDrawPlane(uint8_t *in, uint8_t *out, size_t pitch)
{
    uint16_t *in16 = (uint16_t *)in;
    static const uint32_t palette[4] = {0x000000, 0x55ffff, 0xff55ff, 0xffffff};
    int x, y;

    for (y = 100; y; y--) {
        uint32_t *line1 = (uint32_t*) out;
        uint32_t *line2 = (uint32_t*) (out + pitch);

        /*
         * Draw one word (8 CGA pixels) at a time. With line-doubling
         * and pixel doubling, this is 32 output pixels.
         */

        for (x = 40; x; x--) {
            uint16_t word = *(in16++);

            /* XXX: Not portable to big-endian */

            line1[ 6]=line1[ 7]=line2[ 6]=line2[ 7] = palette[word & 0x3]; word >>= 2;
            line1[ 4]=line1[ 5]=line2[ 4]=line2[ 5] = palette[word & 0x3]; word >>= 2;
            line1[ 2]=line1[ 3]=line2[ 2]=line2[ 3] = palette[word & 0x3]; word >>= 2;
            line1[ 0]=line1[ 1]=line2[ 0]=line2[ 1] = palette[word & 0x3]; word >>= 2;
            line1[14]=line1[15]=line2[14]=line2[15] = palette[word & 0x3]; word >>= 2;
            line1[12]=line1[13]=line2[12]=line2[13] = palette[word & 0x3]; word >>= 2;
            line1[10]=line1[11]=line2[10]=line2[11] = palette[word & 0x3]; word >>= 2;
            line1[ 8]=line1[ 9]=line2[ 8]=line2[ 9] = palette[word & 0x3]; word >>= 2;

            line1 += 16;
            line2 += 16;
        }

        out += pitch << 2;
    }
}

static void
consoleTranslateKey(SDL_Event *event)
{
    int key = event->key.keysym.sym;
    uint8_t scancode = 0;
    uint8_t ascii = 0;

    /*
     * Is it ASCII or a control code? Make it uppercase, and send it.
     */
    if (key <= '~') {
        ascii = key;
        if (key >= 'a' && key <= 'z') {
            ascii ^= 'a' ^ 'A';
        }
    }

    if (event->key.keysym.mod & KMOD_SHIFT) {
        switch (key) {

        case SDLK_UP:      scancode = 0x48; ascii = '8'; break;
        case SDLK_DOWN:    scancode = 0x50; ascii = '2'; break;
        case SDLK_LEFT:    scancode = 0x4B; ascii = '4'; break;
        case SDLK_RIGHT:   scancode = 0x4D; ascii = '6'; break;

        }
    } else {
        switch (key) {

        case SDLK_UP:      scancode = 0x48; break;
        case SDLK_DOWN:    scancode = 0x50; break;
        case SDLK_LEFT:    scancode = 0x4B; break;
        case SDLK_RIGHT:   scancode = 0x4D; break;
        case SDLK_INSERT:  scancode = 0x52; break;
        case SDLK_DELETE:  scancode = 0x53; break;

        }
    }

    if (scancode || key) {
        printf("Console: Received key ascii=%02x scan=%02x\n", ascii, scancode);
        keyboard.ascii = ascii;
        keyboard.scancode = scancode;
        keyboard.eventWaiting = 1;
    }
}

static void
consolePollEvents()
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {

        case SDL_QUIT:
            exit(0);

        case SDL_KEYDOWN:
            consoleTranslateKey(&event);
            break;
        }
    }
}

void
consoleBlitToScreen(uint8_t *fb)
{
    SDL_LockSurface(screen);

    cgaDrawPlane(fb,
                 screen->pixels,
                 screen->pitch);

    cgaDrawPlane(fb + 0x2000,
                 (uint8_t *)screen->pixels + screen->pitch * 2,
                 screen->pitch);

    SDL_UnlockSurface(screen);
    SDL_UpdateRect(screen, 0, 0, 640, 400);

    consolePollEvents();
}
