/*
 * Linker script for the C-language portion of cookinject.
 */

OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)

SECTIONS
{
    /* High main memory, where the bootloader puts us */
    . = 0x02c00000;

    .text : {
        *(.boot);
        *(.text .text.*);
        . = ALIGN(4);
    }

    .data : {
        *(.rodata .rodata.* .data .data.*)
        _edata = .;
        . = ALIGN(4);
    }

    .bss : {
        __bss_start = .;
        *(.bss .bss.*);
        . = ALIGN(4);
    }

    _end = .;

    /DISCARD/ : {
        *(.note .note.* .comment .comment.* .debug* .ARM.attributes);
    }
}
