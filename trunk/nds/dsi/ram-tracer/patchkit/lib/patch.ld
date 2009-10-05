OUTPUT_FORMAT(elf32-littlearm)

PHDRS
{
  /* Ignored by patcher */
  headers PT_PHDR FILEHDR PHDRS ;
  seg_none PT_LOAD FLAGS(0);

  /* Read-only PT_LOAD segments are patched into RAM */
  seg1 PT_LOAD FLAGS(5);
  seg2 PT_LOAD FLAGS(5);
  seg3 PT_LOAD FLAGS(5);
  seg4 PT_LOAD FLAGS(5);
  seg5 PT_LOAD FLAGS(5);
  seg_main PT_LOAD FLAGS(5);
}

SECTIONS
{
  /* Entry points for the launcher (DSi Menu) */
  .launcher_arm7_entry 0x02280200 : { *(.text.launcher_arm7_entry); } :seg1
  .launcher_arm9_entry 0x023fee00 : { *(.text.launcher_arm9_entry); } :seg2

  /* Photo channel entry point */
  .channel_arm7_entry 0x02380000 : { *(.text.channel_arm7_entry); } :seg3

  /* Bottom of main RAM */
  .mainram_base 0x02000000 : { *(.text.mainram_base); } :seg4

  /*
   * Flag that is checked prior to initializing the ARM7 exception vector.
   * If this flag is between 0x02600000 and 0x02800000, the exception vector
   * shouldn't be overwritten by a channel's ARM7 code.
   */
  .arm7_exvector_init_flag 0x02fffd9c : { *(.text.arm7_exvector_init_flag); } :seg5

  /*
   * Patched scratch region: Code and read-only data.
   * This memory acts like it's read-only, since writes go to
   * RAM but reads always come from the patch buffer.
   */
  .p_scratch 0x02ef0000 : {
    *(.text .text.*);
    *(.rodata .rodata.*);
  } :seg_main

  /*
   * Unpatched scratch region: initialized memory (data and bss)
   * XXX: Shouldn't be uninitialized, but we're cheating...
   */
  .unp_scratch 0x02ef8000 : {
    *(.data .data.*);
    *(.bss .bss.*);
  } :seg_none
}
