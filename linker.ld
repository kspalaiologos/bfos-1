ENTRY(entry)

SECTIONS
{
    . = 0x100000;

    .stivalehdr ALIGN(4K) :
    {
        KEEP(*(.stivalehdr))
    }

    .text ALIGN(4K) :
    {
        KEEP(*(.text*))
    }

    .rodata ALIGN(4K) :
    {
        KEEP(*(.rodata*))
    }

    .data ALIGN(4K) :
    {
        KEEP(*(.data*))
    }

    .bss ALIGN(4K) :
    {
        KEEP(*(COMMON))
        KEEP(*(.bss*))
    }
}
