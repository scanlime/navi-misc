/* -*- Mode: C; c-basic-offset: 3 -*- */

#include "types.h"
#include "console_vga.h"
#include "intr.h"
#include "keyboard.h"

volatile uint32 count = 0;

fastcall void
kbHandler(KeyEvent *event)
{
   if (event->key && event->pressed) {
      Console_WriteChar(event->key);
      Console_Flush();
   }
}

int
main(void)
{
   ConsoleVGA_Init();
   Intr_Init();
   Intr_SetFaultHandlers(Console_UnhandledFault);
   Keyboard_Init();
   Keyboard_SetHandler(kbHandler);

   while (1) {
      Intr_Halt();
   }

   return 0;
}
