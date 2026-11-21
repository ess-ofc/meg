OUTPUT_ARCH(aarch64)
ENTRY(_start)

SECTIONS {
    . = 0x40080000;
    __image_start = .;

    .head.text : ALIGN(16K) {
        *(.head.text)
    }

    .text : ALIGN(16K) {
        *(.text.start)
        *(.text .text.*)
    }

    .rodata : ALIGN(16K) {
        *(.rodata .rodata*)
    }

    .data : ALIGN(16K) {
        *(.data .data*)
    }

    .bss : ALIGN(16K) {
        __bss = .;
        *(.bss .bss.*)
        *(COMMON)
        __bss_end = .;
    }

    . = ALIGN(16K);
    __mem_i = .;
    . += (. - __image_start) + 131072;
    __mem_end_i = .;   

    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
        *(.note .note.*)
    }

    __image_end = .;
    __image_size = ABSOLUTE(__image_end - __image_start);
}