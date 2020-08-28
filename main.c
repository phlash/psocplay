#include "cydevice_trm.h"
#include <stdint.h>

// Test symbols in different storage
static const int var_rodata = 0xDEADBEEF;
static int var_data  = 0xFEEDF00D;
static int var_bss;

void main(uint32_t reset) {
    // TBD - initialise the many registers and peripherals we may need...
    // for now: blink the LED ;)
    // drive mode 6 (strong push/pull), alternate data output bit (LSB)
    uint8_t val = (0x6 << 1);
    do {
        *(uint8_t*)(CYREG_PRT2_PC1) = val;
        val ^= 0x01;
        uint64_t delay = 1000000L;
        while (delay>0)
            --delay;
    } while(1);
}
