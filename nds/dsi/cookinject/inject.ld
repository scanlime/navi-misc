/*
 * Linker script for the C-language portion of cookinject.
 */

ENTRY(_start)

SECTIONS
{
    /* High main memory, where the bootloader puts us */
    . = 0x02c00000;

    .text : {
        *(.boot);
        *(.text .text.*);
    }

    .data : {
        *(.rodata .rodata.* .data .data.*)
        _edata = .;
    }

    .bss : {
        __bss_start = .;
        *(.bss .bss.*);
    }

    _end = .;

    /DISCARD/ : {
        *(.note .note.* .comment .comment.* .debug* .ARM.attributes);
    }
}
