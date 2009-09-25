/*
 * Stub for ARM7 and ARM9- get both processors stuck in an infinite
 * loop in working RAM. When this patch works properly, the main RAM
 * bus should be totally quiet afterwards.
 */

asm(".pushsection .text.launcher_arm7_entry \n"
    "  ldr r0, 0f \n"
    "  ldr r1, =0x03000000 \n"
    "  str r0, [r1]\n"
    "  bx  r1 \n"
    "0: \n"
    "  b 0b \n"
    ".popsection \n");

asm(".pushsection .text.launcher_arm9_entry \n"
    "  ldr r0, 0f \n"
    "  ldr r1, =0x03000000 \n"
    "  str r0, [r1]\n"
    "  bx  r1 \n"
    "0: \n"
    "  b 0b \n"
    ".popsection \n");
