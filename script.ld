/* Highest address of the user mode stack */
_estack = 0x20005000;    /* end of RAM */

/* Memories definition */
MEMORY
{
  RAM (xrw)     : ORIGIN = 0x20000000, LENGTH = 20K
  ROM (rx)      : ORIGIN = 0x8000000, LENGTH = 64K
}
/* Sections */
SECTIONS
{
  /* The program code and other data into ROM memory */
  .text :
  {
    . = ALIGN(4);
    *(.vectors)
        KEEP(*(.vectors))
    *(.text)           /* .text sections (code) */
  } >ROM

  /* Constant data into ROM memory*/
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
  } >ROM

  /* Initialized data sections into RAM memory */
  .data :
  {
    . = ALIGN(4);
    *(.data)           /* .data sections */
  } >RAM AT> ROM


  /* Uninitialized data section into RAM memory */
  . = ALIGN(4);
  .bss :
  {
    *(.bss)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >RAM

}
