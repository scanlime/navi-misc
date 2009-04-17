/*
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
#define DEF_WORD_REG(o)           \
    union {                       \
        uint16_t o##x;            \
        struct {                  \
            uint8_t o##l, o##h;   \
        };                        \
    }
#define DEF_FLAG_BITS             \
    uint16_t c : 1;               \
    uint16_t res0 : 1;            \
    uint16_t p : 1;               \
    uint16_t res1 : 1;            \
    uint16_t a : 1;               \
    uint16_t res2 : 1;            \
    uint16_t z : 1;               \
    uint16_t s : 1;               \
    uint16_t t : 1;               \
    uint16_t i : 1;               \
    uint16_t d : 1;               \
    uint16_t o : 1;               \
    uint16_t res3 : 4;
#endif

#if BYTE_ORDER == BIG_ENDIAN
#define DEF_WORD_REG(o)           \
    union {                       \
        uint16_t o##x;            \
        struct {                  \
            uint8_t o##h, o##l;   \
        };                        \
    }
#define DEF_FLAG_BITS             \
    uint16_t t : 1;               \
    uint16_t i : 1;               \
    uint16_t d : 1;               \
    uint16_t o : 1;               \
    uint16_t res3 : 4;            \
    uint16_t c : 1;               \
    uint16_t res0 : 1;            \
    uint16_t p : 1;               \
    uint16_t res1 : 1;            \
    uint16_t a : 1;               \
    uint16_t res2 : 1;            \
    uint16_t z : 1;               \
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

typedef union StackItem {
  uint16_t word;
  jmp_buf addr;
} StackItem;


static void body(Regs reg);
static void videoInit(void);
static void videoPoll(void);
static int videoThread(void *arg);

static uint8_t mem[0xFFFF0 + 0xFFFF];
static SDL_Surface *screen;


/*****************************************************************
 * Misc functions for use in translated code.
 */

static inline
SEG(uint16_t seg, uint16_t off)
{
    /*
     * This needs to be an inline. A macro was provoking gcc to
     * use signed subtraction. for offsets >= 0x8000...
     */
    uint32_t off32 = off;
    uint32_t seg32 = seg;

    /* XXX: Slow down the video so I can see it... */
    if (seg == 0xB800) {
       static uint8_t cnt = 0;
       if (cnt++ == 5) {
          cnt = 0;
          SDL_Delay(1);
       }
    }

    return (seg32 << 4) + off32;
}

static Regs
int10(Regs r)
{
    printf("INT 10, ax=0x%04x\n", r.ax);
    return r;
}

static Regs
int16(Regs r)
{
    printf("INT 16, ax=0x%04x\n", r.ax);
    return r;
}

static Regs
int21(Regs r)
{
    static int numFiles = 0;
    static FILE *files[16];

    /*
     * Silently ignore requests to write out character 0xFF. (invalid)
     * The Robot Odyssey menu seems to do this really often.. why?
     */
    if (r.ah == 0x06 && r.dl == 0xFF) {
        return r;
    }

    printf("INT 21, ax=0x%04x bx=0x%04x cx=0x%04x dx=0x%04x\n", r.ax, r.bx, r.cx, r.dx);

    switch (r.ah) {

    case 0x3D: {
       int fd = numFiles;
       const char *name = mem + SEG(r.ds, r.dx);

       printf("DOS: Open file %04x:%04x='%s' -> #%d\n", r.ds, r.dx, name, fd);

       numFiles++;
       files[fd] = fopen(name, "rb");
       r.ax = fd;

       if (!files[fd]) {
          perror("fopen");
          exit(1);
       }
       break;
    }

    case 0x3E: {
       printf("DOS: Close file #%d\n", r.bx);
       break;
    }

    case 0x3F: {
       int fd = r.bx;
       int len = r.cx;
       void *dest = mem + SEG(r.ds, r.dx);
       int result = fread(dest, 1, len, files[fd]);

       printf("DOS: Read %d bytes from file #%d -> %d bytes at %04x:%04x\n",
              len, fd, result, r.ds, r.dx);
       r.ax = result;
       break;
    }

    }
    return r;
}

static Regs
int3(Regs r)
{
    printf("INT 3\n");
    return r;
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

  videoInit();

  memset(mem, 0, sizeof mem);
  memcpy(mem, dataImage, sizeof dataImage);

  reg.bx = sizeof mem >> 16;      // Memory size (32-bit)
  reg.cx = (uint16_t) sizeof mem;

  reg.ss = 0;                     // XXX: Stack is all fake
  reg.sp = 0xFFFF;

  reg.ds = 0;                     // Beginning of EXE image
  reg.es = 0xFFFF;                // Program Segment Prefix addr

  mem[SEG(reg.es, 0x80)] = 1;     // Empty command line
  mem[SEG(reg.es, 0x81)] = '\0';

  body(reg);
  return 0;
}


/*****************************************************************
 * Video Emulator
 */

static void
videoInit()
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

   SDL_CreateThread(videoThread, NULL);
}

static int
videoThread(void *arg)
{
   while (1) {
      videoPoll();
      SDL_Delay(10);
   }

   return 0;
}

static void
videoDrawPlane(uint8_t *in, uint8_t *out, size_t pitch)
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

         line1[ 6] = line1[ 7] = line2[ 6] = line2[ 7] = palette[word & 0x3]; word >>= 2;
         line1[ 4] = line1[ 5] = line2[ 4] = line2[ 5] = palette[word & 0x3]; word >>= 2;
         line1[ 2] = line1[ 3] = line2[ 2] = line2[ 3] = palette[word & 0x3]; word >>= 2;
         line1[ 0] = line1[ 1] = line2[ 0] = line2[ 1] = palette[word & 0x3]; word >>= 2;
         line1[14] = line1[15] = line2[14] = line2[15] = palette[word & 0x3]; word >>= 2;
         line1[12] = line1[13] = line2[12] = line2[13] = palette[word & 0x3]; word >>= 2;
         line1[10] = line1[11] = line2[10] = line2[11] = palette[word & 0x3]; word >>= 2;
         line1[ 8] = line1[ 9] = line2[ 8] = line2[ 9] = palette[word & 0x3]; word >>= 2;

         line1 += 16;
         line2 += 16;
      }

      out += pitch << 2;
   }
}


static void
videoPoll()
{
   SDL_Event event;

   while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
         exit(0);
      }
   }

   SDL_LockSurface(screen);

   videoDrawPlane(mem + 0xb8000,
                  screen->pixels,
                  screen->pitch);

   videoDrawPlane(mem + 0xb8000 + 0x2000,
                  (uint8_t *)screen->pixels + screen->pitch * 2,
                  screen->pitch);

   SDL_UnlockSurface(screen);
   SDL_UpdateRect(screen, 0, 0, 640, 400);
}
