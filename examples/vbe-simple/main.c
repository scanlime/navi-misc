/* -*- Mode: C; c-basic-offset: 3 -*-
 *
 * Metalkit example: A simple particle system demo, to show off VBE graphics.
 */

#include "vbe.h"
#include "console_vga.h"
#include "math.h"

#define NUM_PARTICLES 32768
#define WIDTH         800
#define HEIGHT        600
#define MAX_AGE       3.0

typedef struct {
   float x, y;
   float vx, vy;
   float age;
} Particle;

Particle particles[NUM_PARTICLES];
uint32 backBuffer[WIDTH * HEIGHT];

static inline uint32
rgb(uint8 r, uint8 g, uint8 b)
{
   return (r << 16) | (g << 8) | b;
}

fastcall static float
prng(void)
{
   /*
    * This is a really tiny approximately 16-bit Park-Miller
    * pseudo-random number generator, which doesn't require any
    * (code-space intensive) 64-bit modulo operation. This uses the
    * same values of n and g used by the ZX Spectrum. (From Wikipedia)
    */
   static uint32 state = 1;
   state = (state * 75) % 65537;
   return state / (float)65537;
}

fastcall static void
init_particle(Particle *p)
{
   memset(p, 0, sizeof *p);

   float theta = (prng() + prng()) * 2 * M_PI;
   float mag = prng();
   p->vx = sinf(theta) * mag;
   p->vy = cosf(theta) * mag;
}

static inline void
draw_and_update_particles(float dt)
{
   int i;

   for (i = 0; i < NUM_PARTICLES; i++) {
      Particle *p = &particles[i];
      int ix = (p->x * 0.5 + 0.5) * WIDTH;
      int iy = (p->y * 0.5 + 0.5) * HEIGHT;
      if (ix >= 0 && iy >= 0 && ix < WIDTH && iy < HEIGHT) {
	 uint8 l = 0xFF - (p->age * (0xFF / MAX_AGE));
	 backBuffer[ix + iy * WIDTH] = rgb(l,l,l);
      }
      p->x += p->vx * dt;
      p->y += p->vy * dt;
      p->age += dt;
      if (p->age > MAX_AGE) {
	 init_particle(p);
      }
   }
}

static inline void
flip(void)
{
   memcpy32(gVBE.current.info.linearAddress, backBuffer, sizeof backBuffer / 4);
   memset32(backBuffer, 0, sizeof backBuffer / 4);
}

int
main(void)
{
   int i;

   ConsoleVGA_Init();
   VBE_InitSimple(800, 600, 32);

   for (i = 0; i < NUM_PARTICLES; i++) {
      particles[i].age = i * (MAX_AGE / (float)NUM_PARTICLES);
   }

   while (1) {
      draw_and_update_particles(0.01);
      flip();
   }

   return 0;
}
