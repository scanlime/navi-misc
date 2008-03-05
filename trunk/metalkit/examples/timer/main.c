/* -*- Mode: C; c-basic-offset: 3 -*- */

#include "types.h"
#include "vgatext.h"
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
   VGAText_Init();
   Intr_Init();
   Intr_SetFaultHandlers(VGAText_DefaultFaultHandler);

   Timer_InitPIT(PIT_HZ / 30);
   Intr_SetMask(0, TRUE);
   Intr_SetHandler(IRQ_TO_VECTOR(0), timerHandler);

   while (1) {
      VGAText_WriteHex(count, 8);
      VGAText_WriteChar('\n');
   }

   return 0;
}
