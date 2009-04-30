#include <nds.h>

static void
isrVCount()
{
   inputGetAndSend();
}

int
main()
{
   irqInit();
   fifoInit();
   readUserSettings();
   initClockIRQ();
   SetYtrigger(80);
   installSystemFIFO();

   irqSet(IRQ_VCOUNT, isrVCount);
   irqEnable(IRQ_VCOUNT);

   while (1) {
      swiWaitForVBlank();
   }

   return 0;
}
