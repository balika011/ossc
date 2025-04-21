SEARCH_DIR(.)
__DYNAMIC  =  0;

MEMORY
{
    FLASH : ORIGIN = 0x10080000, LENGTH = 0x80000
    IRAM  : ORIGIN = 0x10200000, LENGTH = 0x8000
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
    } > FLASH

    .text : {
        . = ALIGN(4);
        _stext = .;
        *(.text)
        *(.text.*)
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
    } > FLASH

    .rvectors :
    {
        . = ALIGN(256);
        _srvectors  =  .;
        KEEP(*(.rvectors))
        . = ALIGN(4);
        _ervectors  =  .;
    } > IRAM AT> FLASH

    .rtext : {
        . = ALIGN(4);
        _srtext  =  .;
        *(.rtext);
        *(.rtext.*)
        . = ALIGN(4);
        _ertext  =  .;
    } > IRAM AT> FLASH

    /*--------------------------------------------------------------------*/
    /* Global constructor/destructor segement                             */
    /*--------------------------------------------------------------------*/

    .preinit_array     :
    {
      PROVIDE_HIDDEN (__preinit_array_start = .);
      KEEP (*(.preinit_array))
      PROVIDE_HIDDEN (__preinit_array_end = .);
    } > FLASH

    .init_array     :
    {
      PROVIDE_HIDDEN (__init_array_start = .);
      KEEP (*(SORT(.init_array.*)))
      KEEP (*(.init_array ))
      PROVIDE_HIDDEN (__init_array_end = .);
    } > FLASH

    .fini_array     :
    {
      PROVIDE_HIDDEN (__fini_array_start = .);
      KEEP (*(SORT(.fini_array.*)))
      KEEP (*(.fini_array ))
      PROVIDE_HIDDEN (__fini_array_end = .);
    } > FLASH

    .rodata : {
        . = ALIGN(4);
        *(.rodata);
        *(.rodata.*)
    } > FLASH

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
    } > IRAM AT> FLASH

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

    _frvectors = LOADADDR(.rvectors);
    _fdata = LOADADDR(.data);
    _frtext = LOADADDR(.rtext);
}

_siram   = ORIGIN(IRAM);
_eiram   = ORIGIN(IRAM) + LENGTH(IRAM) - _min_stack;
