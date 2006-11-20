#include <nds.h>
#include <stdio.h>
#include <stdarg.h>
#include <loader-exit.h>
#include "debug.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_INTERNAL_DEBUG_H

int ft_trace_levels[trace_count];

void Panic(void)
{
  iprintf("\nPanic!\nPress any key for loader exit.\n");

  irqInit();
  irqEnable(IRQ_VBLANK);

  do {
    swiWaitForVBlank();
    scanKeys();
  } while (keysHeld());
  do {
    swiWaitForVBlank();
    scanKeys();
  } while (!keysHeld());

  Loader_Exit9();
  iprintf("No loader found!\n");
  while (1);
}
 
void FT_Message(const char*  fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  viprintf(fmt, ap);
  va_end(ap);
}

void FT_Panic(const char*  fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  viprintf(fmt, ap);
  va_end(ap);

  Panic();
}

void ft_debug_init(void)
{
  int i;
  for (i=0; i<trace_count; i++) {
    ft_trace_levels[i] = 10;
  }
}
