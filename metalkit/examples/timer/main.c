/* -*- Mode: C; c-basic-offset: 3 -*- */

#include "types.h"
#include "console_vga.h"
#include "timer.h"
#include "intr.h"

volatile uint32 count = 0;

void
timerHandler(int vector)
{
   count++;
}

int
main(void)
{
   ConsoleVGA_Init();
   Intr_Init();
   Intr_SetFaultHandlers(Console_UnhandledFault);

   Timer_InitPIT(PIT_HZ / 100);
   Intr_SetMask(0, TRUE);
   Intr_SetHandler(IRQ_VECTOR(0), timerHandler);

   while (1) {
      Console_MoveTo(0, 0);
      Console_Format("1/100 seconds: %d", count);
      Console_Flush();
   }

   return 0;
}
