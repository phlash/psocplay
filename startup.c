// Trivial startup code..
#include "cydevice_trm.h"
#include <stdint.h>

// symbols from ldscript
extern char __etext, __sdata, __edata, __sbss, __ebss, __eram;

// expected app entry point
extern void main(uint32_t reset);

// test variables in different sections
const static int var_in_rodata=0x9876;
static int var_in_data=0x1234;
static int var_in_bss;

// what it says..
int _hang() {
	while(1);
}

// our entry point (see below)
int _start() {
	// grab reset registers (4x8bit)
	uint32_t reset = *(uint32_t*)(CYREG_RESET_SR0);
	// copy declared data to RAM
	char *src = &__etext;
	char *dst = &__sdata;
	while (dst < &__edata)
		*dst++ = *src++;
	// zero BSS
	for (dst = &__sbss; dst < &__ebss; dst++)
		*dst = 0;
	// fire up the app, pass on reset cause
	main(reset);
	// hang for now... TBD
	_hang();
}

// Cortex-M3 minimal vector table - linked first due to section name :)
void *vectors[4] __attribute__((section(".vectors"))) = {
	&__eram,	// initial stack pointer
	_start,		// reset vector
	_hang,		// NMI
	_hang		// hard fault
};
