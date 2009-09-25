/*
 * Stub for ARM7 and ARM9- get both processors stuck in an infinite loop.
 */

asm(".pushsection .text.launcher_arm7_entry \n"
    "0: \n"
    "  b 0b \n"
    ".popsection \n");

asm(".pushsection .text.launcher_arm9_entry \n"
    "0: \n"
    "  b 0b \n"
    ".popsection \n");
