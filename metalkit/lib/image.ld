/*
 * GNU Linker script for assembling a Metalkit binary image.
 *
 * Notable changes from ld's default behaviour:
 *
 *   - Load address is at the 1MB boundary.
 *
 *   - Our binary begins with a .boot section.
 *
 *   - The end of the data section is padded to a
 *     512-byte boundary, to make sure that our disk
 *     image ends on a sector boundary. (Required by QEMU)
 */

OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start)

SECTIONS
{
   . = 0x100000;

   .text : {
      _file_origin = .;
      *(.boot);
      *(.text .text.*);
    }

   .data : {
      *(.rodata .rodata.* .data .data.*)
      _edata = .;

      _sector_padding = .;
      . = ALIGN(512);
      _sector_padding_end = .;
   }

   .bss : {
      __bss_start = .;
      *(.bss .bss.*);
   }

   _end = .;
}