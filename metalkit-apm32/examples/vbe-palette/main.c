/* -*- Mode: C; c-basic-offset: 3 -*-
 *
 * Metalkit example: Demonstrate color palette manipulation in VBE mode.
 */

#include "vbe.h"
#include "console_vga.h"
#include "timer.h"
#include "intr.h"
#include "math.h"


/*
 * timerISR --
 *
 *    Timer interrupt handler, which animates the palette.
 *
 *    Just for fun, we'll actually use our palette as a low-res 16x16
 *    pixel true-color framebuffer, and well draw some abstract art.
 */

static void
timerISR(int vector)
{
   uint32 palette[256];
   static int tick = 0;
   int i;

   /*
    * Let the phase of each color channel slowly drift around.
    */
   const float rPhase = tick * 0.001;
   const float gPhase = tick * 0.002;
   const float bPhase = tick * 0.003;

   for (i = 0; i < 256; i++) {
      const int x = (i & 0x0F) - 3;
      const int y = (i >> 4) - 3;
      const float t = (x*x + y*y) * 0.05 + tick * 0.02;

      const uint8 r = sinf(t + rPhase) * 0x0f + 0x10;
      const uint8 g = sinf(t + gPhase) * 0x0f + 0x10;
      const uint8 b = sinf(t + bPhase) * 0x0f + 0x10;

      palette[i] = (r << 16) | (g << 8) | b;
   }

   VBE_SetPalette(0, 256, palette);
   tick++;
}


/*
 * drawTestPattern --
 *
 *    Visually present the palette as a 16x16 grid of boxes.
 */

static inline void
drawTestPattern(void)
{
   uint8 *fb = (uint8*) gVBE.current.info.linearAddress;
   int bytesPerLine = gVBE.current.info.bytesPerLine;
   int boxWidth = gVBE.current.info.width >> 4;
   int boxHeight = gVBE.current.info.height >> 4;
   int x, y;

   for (y = 0; y < gVBE.current.info.height; y++) {
      uint8 *line = fb;
      fb += bytesPerLine;

      for (x = 0; x < gVBE.current.info.width; x++) {
         int row = y / boxHeight;
         int col = x / boxWidth;
         int color = (row << 4) | col;

         *(line++) = color;
      }
   }
}


int
main(void)
{
   ConsoleVGA_Init();
   Intr_Init();
   Intr_SetFaultHandlers(Console_UnhandledFault);

   VBE_InitSimple(640, 480, 8);
   drawTestPattern();

   /*
    * Start our timer ISR, which will animate the palette at 60 Hz.
    */
   Timer_InitPIT(PIT_HZ / 60);
   Intr_SetMask(PIT_IRQ, TRUE);
   Intr_SetHandler(IRQ_VECTOR(PIT_IRQ),timerISR);

   /*
    * Sleep between interrupts.
    */
   while (1) {
      Intr_Halt();
   }

   return 0;
}
