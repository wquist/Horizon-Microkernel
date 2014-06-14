OUTPUT_FORMAT(elf32-i386)
ENTRY(bootstrap)

_APSETUP_PHYS_ADDR = 0x00080000;

_KERNEL_PHYS_ADDR = 0x00100000;
_KERNEL_VIRT_ADDR = 0xC0000000;

SECTIONS
{
    .setup _KERNEL_PHYS_ADDR : AT( ADDR(.setup) )
    {
        ___skernel = .;

        ___ssetup = .;
        *(.setup)
        ___esetup = .;

        . = ALIGN(4096);

        ___sapsetup = .;
        *(.apsetup)
        ___eapsetup = .;

        . = ALIGN(4096);
    }

    .kernel ( . + _KERNEL_VIRT_ADDR - _KERNEL_PHYS_ADDR ) : AT( ADDR(.kernel) - _KERNEL_VIRT_ADDR + _KERNEL_PHYS_ADDR )
    {
        *(.kentry)
        
        . = ALIGN(4096);
        
        *(.isrs)
        
        . = ALIGN(4096);

        *(.text)

        . = ALIGN(4096);
    }

    .data : AT( ADDR(.data) - _KERNEL_VIRT_ADDR + _KERNEL_PHYS_ADDR )
    {
        ___sdata = .;
        *(.data)
        ___edata = .;

        . = ALIGN(4096);
    }

    .rodata : AT( ADDR(.rodata) - _KERNEL_VIRT_ADDR + _KERNEL_PHYS_ADDR )
    {
        ___srodata = .;
        *(.rodata)
        ___erodata = .;

        . = ALIGN(4096);
    }

    .bss : AT( ADDR(.bss) - _KERNEL_VIRT_ADDR + _KERNEL_PHYS_ADDR )
    {
        ___sbss = .;
        *(.bss)

        . = ALIGN(4096);

        *(COMMON)
        ___ebss = .;

        . = ALIGN(4096);

        ___ekernel = .;
    }
}