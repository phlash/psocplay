// Trivial startup code..

// symbols from ldscript
extern char __etext, __sdata, __edata, __sbss, __ebss, __eram;

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
	// copy declared data to RAM
	char *src = &__etext;
	char *dst = &__sdata;
	while (dst < &__edata)
		*dst++ = *src++;
	// zero BSS
	for (dst = &__sbss; dst < &__ebss; dst++)
		*dst = 0;
	// hang for now... TBD
	_hang();
}

// Cortex-M3 minimal vector table - linked first due to section name :)
void *vectors[3] __attribute__((section(".vectors"))) = {
	&__eram,	// initial stack pointer
	_start,		// reset vector
	_hang		// hard fault
};
