OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

SECTIONS
{
    .text 0x1000000 :
    {
        *(.text)
        . = ALIGN(4096);
    }
    
    .data :
    {
        *(.data)
        . = ALIGN(4096);
    }

    .rodata :
    {
        *(.rodata)
        . = ALIGN(4096);
    }

    .bss :
    {
        *(.bss)
        *(COMMON)
        . = ALIGN(4096);
    }
}