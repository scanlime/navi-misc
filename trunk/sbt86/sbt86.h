/* -*- Mode: C; c-basic-offset: 4 -*-
 *
 * Boilerplate code and data types for sbt86, an
 * experimental 8086 -> C static binary translator.
 *
 * Micah Dowty <micah@navi.cx>
 */

#include <stdint.h>
#include <endian.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <stdlib.h>
#include <SDL.h>


/*****************************************************************
 * Definitions
 */

#if BYTE_ORDER == LITTLE_ENDIAN
#define DEF_WORD_REG(o)                         \
    union {                                     \
        uint16_t o##x;                          \
        struct {                                \
            uint8_t o##l, o##h;                 \
        };                                      \
    }
#define DEF_FLAG_BITS                           \
    uint16_t c : 1;                             \
    uint16_t res0 : 1;                          \
    uint16_t p : 1;                             \
    uint16_t res1 : 1;                          \
    uint16_t a : 1;                             \
    uint16_t res2 : 1;                          \
    uint16_t z : 1;                             \
    uint16_t s : 1;                             \
    uint16_t t : 1;                             \
    uint16_t i : 1;                             \
    uint16_t d : 1;                             \
    uint16_t o : 1;                             \
    uint16_t res3 : 4;
#endif

#if BYTE_ORDER == BIG_ENDIAN
#define DEF_WORD_REG(o)                         \
    union {                                     \
        uint16_t o##x;                          \
        struct {                                \
            uint8_t o##h, o##l;                 \
        };                                      \
    }
#define DEF_FLAG_BITS                           \
    uint16_t t : 1;                             \
    uint16_t i : 1;                             \
    uint16_t d : 1;                             \
    uint16_t o : 1;                             \
    uint16_t res3 : 4;                          \
    uint16_t c : 1;                             \
    uint16_t res0 : 1;                          \
    uint16_t p : 1;                             \
    uint16_t res1 : 1;                          \
    uint16_t a : 1;                             \
    uint16_t res2 : 1;                          \
    uint16_t z : 1;                             \
    uint16_t s : 1;
#endif

typedef struct Regs {
    DEF_WORD_REG(a);
    DEF_WORD_REG(b);
    DEF_WORD_REG(c);
    DEF_WORD_REG(d);
    uint16_t si, di;
    uint16_t cs, ds, es, ss;
    uint16_t sp, bp;
} Regs;

typedef struct Flags {
    union {
        struct {
            DEF_FLAG_BITS
        };
        uint16_t value;
    };
} Flags;

typedef struct IntState {
    Regs reg;
    Flags flag;
} IntState;

typedef union StackItem {
    uint16_t word;
    jmp_buf addr;
} StackItem;


static void body(Regs reg);
static void consoleInit(void);
static void consolePoll(void);
static int consoleThread(void *arg);

static uint8_t mem[0xFFFF0 + 0xFFFF];
static SDL_Surface *screen;

static volatile struct {
    int eventWaiting;
    int scancode;
    int ascii;
} keyboard;


/*****************************************************************
 * Misc functions for use in translated code.
 */

static inline
SEG(uint16_t seg, uint16_t off)
{
    /*
     * This needs to be an inline. A macro was provoking gcc to
     * use signed subtraction for offsets >= 0x8000...
     */
    uint32_t off32 = off;
    uint32_t seg32 = seg;

    return (seg32 << 4) + off32;
}

static void
int10(IntState *s)
{
    switch (s->reg.ah) {

    case 0x00: {              /* Set video mode */
        /* Ignore. We're always in CGA mode. */
        break;
    }

    default: {
        printf("BIOS10: Unsupported! ax=0x%04x\n", s->reg.ax);
        break;
    }
    }
}

static void
int16(IntState *s)
{
    switch (s->reg.ah) {

    case 0x00: {              /* Get keystroke */
        if (keyboard.eventWaiting) {
            s->reg.ah = keyboard.scancode;
            s->reg.al = keyboard.ascii;
            printf("BIOS16: Got key %04x\n", s->reg.ax);
            keyboard.eventWaiting = 0;
        } else {
            s->reg.ax = 0;
        }
        break;
    }

    case 0x01: {              /* Check for keystroke */
        if (keyboard.eventWaiting) {
            s->flag.z = 0;
            s->reg.ah = keyboard.scancode;
            s->reg.al = keyboard.ascii;
        } else {
            s->flag.z = 1;
        }
        break;
    }

    default: {
        printf("BIOS16: Unsupported! ax=0x%04x\n", s->reg.ax);
        break;
    }
    }
}

static void
int21(IntState *s)
{
    static int numFiles = 0;
    static FILE *files[16];

    switch (s->reg.ah) {

    case 0x06: {              /* Direct console input/output (Only input supported) */
        if (s->reg.dl == 0xFF) {
            if (keyboard.eventWaiting) {
                s->reg.al = keyboard.ascii;
                s->flag.z = 0;
                keyboard.eventWaiting = 0;
            } else {
                s->reg.al = 0;
                s->flag.z = 1;
            }
        }
        break;
    }

    case 0x25: {              /* Set interrupt vector */
        /* Ignored. Robot Odyssey uses this to set the INT 24h error handler. */
        break;
    }

    case 0x3D: {              /* Open File */
        int fd = numFiles;
        const char *name = mem + SEG(s->reg.ds, s->reg.dx);

        printf("DOS: Open file %04x:%04x='%s' -> #%d\n",
               s->reg.ds, s->reg.dx, name, fd);

        numFiles++;
        files[fd] = fopen(name, "rb");
        s->reg.ax = fd;
        s->flag.c = 0;

        if (!files[fd]) {
            perror("fopen");
            exit(1);
        }
        break;
    }

    case 0x3E: {              /* Close File */
        printf("DOS: Close file #%d\n", s->reg.bx);
        s->flag.c = 0;
        break;
    }

    case 0x3F: {              /* Read File */
        int fd = s->reg.bx;
        int len = s->reg.cx;
        void *dest = mem + SEG(s->reg.ds, s->reg.dx);
        int result = fread(dest, 1, len, files[fd]);

        printf("DOS: Read %d bytes from file #%d -> %d bytes at %04x:%04x\n",
               len, fd, result, s->reg.ds, s->reg.dx);
        s->reg.ax = result;
        s->flag.c = 0;
        break;
    }

    case 0x4C: {              /* Exit with return code */
        int retval = s->reg.al;
        printf("DOS: Exit (Return code %d)\n", retval);
        exit(retval);
        break;
    }

    default:
        printf("DOS: Unsupported! ax=0x%04x\n", s->reg.ax);
        break;
    }
}

static Regs
int3(IntState *s)
{
    //printf("INT 3\n");
}

static uint8_t
in(uint16_t port)
{
    printf("IN %04x\n", port);
    return 0;
}

static void
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
    register Regs reg = {{ 0 }};
    const char *cmdLine = argc > 1 ? argv[1] : "";

    consoleInit();

    memset(mem, 0, sizeof mem);
    memcpy(mem, dataImage, sizeof dataImage);

    reg.bx = sizeof mem >> 16;      // Memory size (32-bit)
    reg.cx = (uint16_t) sizeof mem;

    reg.ss = 0;                     // XXX: Stack is all fake
    reg.sp = 0xFFFF;

    reg.ds = 0;                     // Beginning of EXE image (unrelocated)
    reg.es = 0xFFFF;                // Program Segment Prefix addr

    mem[SEG(reg.es, 0x80)] = strlen(cmdLine);
    strcpy(mem + SEG(reg.es, 0x81), cmdLine);

    body(reg);
    return 0;
}


/*****************************************************************
 * Console Emulator
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

    SDL_CreateThread(consoleThread, NULL);
}

static int
consoleThread(void *arg)
{
    while (1) {
        consolePoll();
        SDL_Delay(10);
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

    switch (key) {

    case SDLK_UP:    scancode = 0x83; break;
    case SDLK_DOWN:  scancode = 0x84; break;
    case SDLK_LEFT:  scancode = 0x79; break;
    case SDLK_RIGHT: scancode = 0x89; break;

    }

    if (scancode || key) {
        printf("Console: Received key ascii=%02x scan=%02x\n", ascii, scancode);
        keyboard.ascii = ascii;
        keyboard.scancode = scancode;
        keyboard.eventWaiting = 1;
    }
}


static void
consolePoll()
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

    SDL_LockSurface(screen);

    cgaDrawPlane(mem + 0xb8000,
                 screen->pixels,
                 screen->pitch);

    cgaDrawPlane(mem + 0xb8000 + 0x2000,
                 (uint8_t *)screen->pixels + screen->pitch * 2,
                 screen->pitch);

    SDL_UnlockSurface(screen);
    SDL_UpdateRect(screen, 0, 0, 640, 400);
}
