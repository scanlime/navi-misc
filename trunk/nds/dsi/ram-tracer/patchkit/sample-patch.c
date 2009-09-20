/*
 * Sample patch file.
 *
 * Shows how to invoke your patch, and how to use IOHook.
 */

#include "iohook.h"

asm(".pushsection .text.launcher_arm7_entry \n"
    "  b main \n"
    ".popsection \n");

int
main()
{
   int i;
   uint8_t buffer[256];

   IOHook_Init();

   IOHook_LogStr("Hello World");

   IOHook_FOpenW("sample-output.bin\0\0");

   for (i = 0; i < sizeof buffer; i++) {
      buffer[i] = i;
   }
   IOHook_FWrite(buffer, sizeof buffer);

   IOHook_FSeek(0);
   IOHook_LogHex(buffer, IOHook_FRead(buffer, sizeof buffer));

   IOHook_Quit("Done!");
}
