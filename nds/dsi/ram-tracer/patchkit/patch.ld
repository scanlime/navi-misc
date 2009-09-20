OUTPUT_FORMAT(elf32-littlearm)

PHDRS
{
  headers PT_PHDR FILEHDR PHDRS ;
  sec_none PT_NULL;
  sec1 PT_LOAD;
  sec2 PT_LOAD;
  sec3 PT_LOAD;
}

SECTIONS
{
  .launcher_arm7_entry 0x02280200 : { *(.text.launcher_arm7_entry); } :sec1
  .launcher_arm9_entry 0x023fee00 : { *(.text.launcher_arm9_entry); } :sec2

  /*
   * Patched scratch region: Code and read-only data.
   * This memory acts like it's read-only, since writes go to
   * RAM but reads always come from the patch buffer.
   */
  .p_scratch 0x02ef0000 : {
    *(.text .text.*);
    *(.rodata .rodata.*);
  } :sec3

  /*
   * Unpatched scratch region: initialized memory (data and bss)
   * XXX: Shouldn't be uninitialized, but we're cheating...
   */
  .unp_scratch 0x02ef8000 : {
    *(.data .data.*);
    *(.bss .bss.*);
  } :sec_none
}
