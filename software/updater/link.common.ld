SEARCH_DIR(.)
__DYNAMIC  =  0;

MEMORY
{
    FRAM : ORIGIN = 0xA0000000, LENGTH = 0x3000
    IRAM : ORIGIN = 0xA0003000, LENGTH = 0x3000
}

/* Stack information variables */
_min_stack      = 0x1000;   /* 4KB - minimum stack space to reserve */
_stack_start   = ORIGIN(IRAM) + LENGTH(IRAM);

/* We have to align each sector to word boundaries as our current s19->slm
 * conversion scripts are not able to handle non-word aligned sections. */

SECTIONS
{
    .vectors :
    {
        KEEP(*(.vectors))
    } > FRAM

    .text : {
        . = ALIGN(4);
        _stext = .;
        *(.text)
        *(.text.*)
        *(.rtext);
        *(.rtext.*)
        _etext  =  .;
        __CTOR_LIST__ = .;
        LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2)
        *(.ctors)
        LONG(0)
        __CTOR_END__ = .;
        __DTOR_LIST__ = .;
        LONG((__DTOR_END__ - __DTOR_LIST__) / 4 - 2)
        *(.dtors)
        LONG(0)
        __DTOR_END__ = .;
        *(.lit)
        *(.shdata)
        _etext = .;
    } > FRAM

    /*--------------------------------------------------------------------*/
    /* Global constructor/destructor segement                             */
    /*--------------------------------------------------------------------*/

    .preinit_array     :
    {
      PROVIDE_HIDDEN (__preinit_array_start = .);
      KEEP (*(.preinit_array))
      PROVIDE_HIDDEN (__preinit_array_end = .);
    } > FRAM

    .init_array     :
    {
      PROVIDE_HIDDEN (__init_array_start = .);
      KEEP (*(SORT(.init_array.*)))
      KEEP (*(.init_array ))
      PROVIDE_HIDDEN (__init_array_end = .);
    } > FRAM

    .fini_array     :
    {
      PROVIDE_HIDDEN (__fini_array_start = .);
      KEEP (*(SORT(.fini_array.*)))
      KEEP (*(.fini_array ))
      PROVIDE_HIDDEN (__fini_array_end = .);
    } > FRAM

    .rodata : {
        . = ALIGN(4);
        *(.rodata);
        *(.rodata.*)
    } > FRAM

    .data : {
        . = ALIGN(4);
        _sdata  =  .;
        *(.data);
        *(.data.*)
        *(.sdata);
        *(.sdata.*)
        *(.sdata2);
        *(.sdata2.*)
        _edata  =  .;
    } > FRAM

    .bss :
    {
        . = ALIGN(4);
        _bss_start = .;
        *(.bss)
        *(.bss.*)
        *(.sbss)
        *(.sbss.*)
        *(COMMON)
        _bss_end = .;
    } > IRAM

    /* ensure there is enough room for stack */
    .stack (NOLOAD): {
        . = ALIGN(4);
        . = . + _min_stack ;
        . = ALIGN(4);
        stack = . ;
        _stack = . ;
    } > IRAM

    .stab  0 (NOLOAD) :
    {
        [ .stab ]
    }

    .stabstr  0 (NOLOAD) :
    {
        [ .stabstr ]
    }

    _fdata = LOADADDR(.data);
}

_siram   = ORIGIN(IRAM);
_eiram   = ORIGIN(IRAM) + LENGTH(IRAM) - _min_stack;
