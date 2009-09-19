OUTPUT_FORMAT(elf32-littlearm)

PHDRS
{
  headers PT_PHDR FILEHDR PHDRS ;
  seg1 PT_LOAD;
  seg2 PT_LOAD;
  seg3 PT_LOAD;
}

SECTIONS
{
  .launcher_arm7_entry 0x280200 : { *(.text.launcher_arm7_entry); } :seg1
  .launcher_arm9_entry 0x3fee00 : { *(.text.launcher_arm9_entry); } :seg2
  .scratch 0xef0000 : { *(.text .text.*); } :seg3
}
