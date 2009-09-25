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
  seg_main PT_LOAD FLAGS(5);
}

SECTIONS
{
  .launcher_arm7_entry 0x02280200 : { *(.text.launcher_arm7_entry); } :seg1
  .launcher_arm9_entry 0x023fee00 : { *(.text.launcher_arm9_entry); } :seg2
  .mainram_base 0x02000000  : { *(.text.mainram_base); } :seg3

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
