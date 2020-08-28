/* LD script for Cypress PSoC 5LP with cortex-m3 MCU, based on the datasheet memory map:
 * https://www.cypress.com/file/45906/download
 */
ENTRY(_start);
MEMORY {
	FLASH(rx)   : ORIGIN = 0x0, LENGTH = 0x40000 /* 256k */
	/* VECTRAM+KERNRAM => CODERAM block: 32k through icache */
	VECTRAM(wx) : ORIGIN = 0x1FFF8000, LENGTH = (48*4)
	KERNRAM(wx) : ORIGIN = 0x1FFF8000+(48*4), LENGTH = 0x8000-(48*4)
	HEAPRAM(w)  : ORIGIN = 0x20000000, LENGTH = 0x8000 /* 32k, not through icache */
	BITBAND(w)  : ORIGIN = 0x22000000, LENGTH = 0x100000 /* 1M bitband aliases to DATARAM */
}

SECTIONS {
	/* ROM vector tables go first, then .text* segments */
	.text : { *(.romvector*); *(.text*); } >FLASH
	/* read-only data which stays in FLASH */
	.rodata : { *(.rodata*); __etext = .; } >FLASH
	/* initialised kernel data that needs to be copied out to KERNRAM from FLASH */
	.data : { __sdata = .; *(.data*); __edata = .; } >KERNRAM AT>FLASH
	/* uninitialised data that needs to be zeroed for C */
	.bss : { __sbss = .; *(.bss*); __ebss = .; } >KERNRAM
	/* variables declared to be on the heap */
	.heap : { __sheap = .; *(.heap*); __eheap = .; } > HEAPRAM
	/* symbol for RAM vector table */
	__ramvectors = ORIGIN(VECTRAM);
	/* symbol for end of RAM */
	__eram = ORIGIN(HEAPRAM) + LENGTH(HEAPRAM);
}
