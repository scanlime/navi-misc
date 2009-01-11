/* -*- Mode: C; c-basic-offset: 3 -*- */

#include "types.h"
#include "console_vga.h"
#include "intr.h"
#include "keyboard.h"

volatile uint32 count = 0;

fastcall void
kbHandler(KeyEvent *event)
{
   Console_Format("scancode: %02x  key: %02x '%c'  pressed: %02x\n",
		  event->scancode,
		  event->key, event->key,
		  event->pressed);
   Console_Flush();
}

int
main(void)
{
   ConsoleVGA_Init();
   Intr_Init();
   Intr_SetFaultHandlers(Console_UnhandledFault);
   Keyboard_Init();
   Keyboard_SetHandler(kbHandler);

   Console_Format("Keyboard Test\n");
   Console_Flush();

   while (1);

   return 0;
}
