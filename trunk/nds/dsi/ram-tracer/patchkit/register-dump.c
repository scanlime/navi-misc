/*
 * Patch which dumps registers out to the console.
 *
 * Modify the section name below and/or the linker script in order to
 * insert the patch at different addresses.
 *
 * --Micah Dowty <micah@navi.cx>
 */

#define HOOK_SECTION_NAME  ".text.launcher_arm7_entry"

#include "iohook.h"

asm(".pushsection " HOOK_SECTION_NAME "\n"
    "  push {r0-r12,r14-r15} \n"    // Save all registers except SP
    "  mov r0, r13 \n"              // Pass SP as argument to main()
    "  b main \n"
    ".popsection \n");

int
main(uint32_t *sp)
{
   int i;

   IOHook_Init();

   IOHook_LogStr("Registers r0-r15:");
   for (i = 0; i <= 12; i++) {
      IOHook_LogHex(&sp[i], sizeof *sp);
   }
   IOHook_LogHex((uint32_t*) &sp, sizeof sp); // SP is special.
   for (i = 14; i <= 15; i++) {
      IOHook_LogHex(&sp[i-1], sizeof *sp);
   }

   IOHook_LogStr("Top of stack:");
   IOHook_LogHex(sp + 15, 7*4*32);

   IOHook_Quit("Done!");
}
