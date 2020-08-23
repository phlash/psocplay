/* LD script for Cypress PSoC 5LP with cortex-m3 MCU, based on the datasheet memory map:
 * https://www.cypress.com/file/45906/download
 */
ENTRY(_start);
MEMORY {
	FLASH(rx)   : ORIGIN = 0x0, LENGTH = 0x40000 /* 256k */
	CODERAM(wx) : ORIGIN = 0x1FFF8000, LENGTH = 0x8000 /* 32k, attached to icache */
	DATARAM(w)  : ORIGIN = 0x20000000, LENGTH = 0x8000 /* 32k, outside icache */
	BITBAND(w)  : ORIGIN = 0x22000000, LENGTH = 0x100000 /* 1M bitband aliases to DATARAM */
}

SECTIONS {
	/* vector tables go first, then .text* segments */
	.text : { *(.vector*); *(.text*); } >FLASH
	/* now read-only data which stays in FLASH */
	.rodata : { *(.rodata*); __etext = .; } >FLASH
	/* now declared data that needs to be copied out to RAM from FLASH */
	.data : { __sdata = .; *(.data*); __edata = .; } >DATARAM AT>FLASH
	/* finally undeclared data that needs to be zeroed for C */
	.bss : { __sbss = .; *(.bss*); __ebss = .; } >DATARAM
	/* symbol for end of RAM */
	__eram = ORIGIN(DATARAM) + LENGTH(DATARAM);
}
