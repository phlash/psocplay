// Trivial startup code..
#include "cydevice_trm.h"
#include <stdint.h>

// symbols from ldscript
extern char __etext, __sdata, __edata, __sbss, __ebss, __ramvectors, __eram;

// expected app entry point
extern void main(uint32_t reset);

// test variables in different sections
const static int var_in_rodata=0x9876;
static int var_in_data=0x1234;
static int var_in_bss;

// Cortex-M3 minimal ROM vector table - section name ensures link address is correct (0x0)
void _hang();
void _start();
#define NROMVECT 4
#define NRAMVECT 48
static void *__romvectors[NROMVECT] __attribute__((section(".romvectors"))) = {
	&__eram,	// initial stack pointer
	_start,		// reset vector
	_hang,		// NMI
	_hang		// hard fault
};

// what it says..
void _hang() {
	while(1);
}

// default exception handler, go _hang!
void _except() {
	_hang();
}

// our entry point (see below)
void _start() {
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
	// copy ROM vector table to RAM, default most handlers to _except
	int vect;
	void **ramv = (void**)&__ramvectors;
	for (vect=0; vect<NRAMVECT; vect++) {
		ramv[vect] = (vect<NROMVECT) ? __romvectors[vect] : _except;
	}
	*(uint32_t*)(CYREG_NVIC_VECT_OFFSET) = (uint32_t)ramv;
	// fire up the app, pass on reset cause
	main(reset);
	// hang for now... TBD
	_hang();
}
