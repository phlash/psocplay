// Trivial startup code..
#include "cydevice_trm.h"
#include <stdint.h>

// symbols from ldscript
extern char __etext, __sdata, __edata, __sbss, __ebss, __sheap, __eheap, __eram, __ramvectors;

// expected app entry point
extern void main(uint32_t reset);

// Cortex-M3 minimal ROM vector table - section name ensures link address is correct (0x0)
void _except();
void _start();
#define NROMVECT 4
#define NRAMVECT 48
static void *__romvectors[NROMVECT] __attribute__((section(".romvectors"))) = {
	&__sheap,	// initial stack pointer (below heap)
	_start,		// reset vector
	_except,	// NMI
	_except		// hard fault
};

// what it says..
void _hang() {
	while(1)
		__asm("wfi");	// save some power while we wait..
}

// default exception handler, go _hang!
void _except() {
	// grab exception number for debugger
	uint8_t exno = (uint8_t)(*(uint32_t*)(CYREG_NVIC_INTR_CTRL_STATE));
	_hang();
}

// our entry point (see above)
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

// set an exception vector in RAM
void *_setvector(int num, void *value) {
	void **ramv = (void**)&__ramvectors;
	void *prev = (void *)0;
	if (num<NRAMVECT) {
		prev = ramv[num];
		ramv[num] = value;
	}
	return prev;
}
