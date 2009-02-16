/* -*- Mode: C; c-basic-offset: 3 -*- */

#include "types.h"
#include "console_vga.h"
#include "intr.h"
#include "keyboard.h"
#include "apm.h"

int
main(void)
{
   ConsoleVGA_Init();
   Intr_Init();
   Intr_SetFaultHandlers(Console_UnhandledFault);
   Keyboard_Init();
   APM_Init();

   Console_Format("APM Version %2x.%02x, Flags %04x, connected=%d\n"
                  "\n"

                  // XXX: APM_Idle is currently broken.
                  // "Running in low-power APM Idle mode now.\n"
                  // "\n"

                  "Key commands:\n"
                  "\n"
                  " 0 - S0 (On)\n"
                  " 1 - S1 (Standby)\n"
                  " 2 - S2 (Suspend)\n"
                  " 3 - S3 (Off)\n"
                  "\n",
                  gAPM.version >> 8, gAPM.version & 0xFF,
                  gAPM.flags, gAPM.connected);
   Console_Flush();

   while (1) {
      int i;

      APM_Idle();

      for (i = 0; i < 4; i++) {
         if (Keyboard_IsKeyPressed('0' + i)) {
            while (Keyboard_IsKeyPressed('0' + i)) {
               APM_Idle();
            }

            Console_Format("Setting power state %d... ", i);
            Console_Flush();

            APM_SetPowerState(i);

            Console_Format("Done\n");
            Console_Flush();
         }
      }
   }

   return 0;
}
