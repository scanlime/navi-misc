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
   const int count = 256;
   uint32_t buffer[count + IOH_PAD32];

   IOHook_Init();

   IOHook_LogStr("Hello World");

   IOHook_FOpenW("sample-output.bin");

   for (i = 0; i < count; i++) {
      buffer[i] = i;
   }
   IOHook_FWrite(buffer, count * sizeof buffer[0]);

   IOHook_FSeek(0);
   IOHook_FRead(buffer, count * sizeof buffer[0]);
   IOHook_LogHex(buffer, count * sizeof buffer[0]);

   IOHook_Quit("Done!");
}
