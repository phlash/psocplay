// wire up FreeRTOS to the hardware, say hello via LED :)
#include <FreeRTOS.h>
#include <task.h>

// vector stuff in startup.c
extern void _setvector(int num, void *value);

// define the allocation heap in special section
uint8_t ucHeap[ configTOTAL_HEAP_SIZE ] __attribute((section(".heap")));

// dummy failure hooks
extern void _hang(void);
void vApplicationStackOverflowHook(TaskHandle_t tHnd, char *tName) { _hang(); }
void vApplicationMallocFailedHook(void) { _hang(); }

// idle task hook
void vApplicationIdleHook(void) {}
// sys tick hook
static volatile int hookSaysNo;
static int ticks;
void vApplicationTickHook(void) {
	// stop wiggling every 5 secs, for 5secs
	if ((++ticks)%5000==0)
		hookSaysNo = ~hookSaysNo;
}

// dummy stats timer (high resolution timer) - intending to read systick downcounter..
void vApplicationConfigStatsTimer(void) {}
unsigned long vApplicationGetStatsTimer(void) { return 0L; }

// exception handlers in the CM3 port and where they go..
extern void xPortPendSVHandler(void);
#define PEND_SV_EXC	14
extern void xPortSysTickHandler(void);
#define SYSTICK_EXC	15
extern void vPortSVCHandler(void);
#define SYSCALL_EXC	11

// LED wiggle task
static void wiggle(void *params) {
    uint8_t val = (0x6 << 1);
    do {
        *(uint8_t*)(CYREG_PRT2_PC1) = val;
		if (!hookSaysNo)
        	val ^= 0x01;
        vTaskDelay(500);    // ticks@1kHz, thus 0.5sec
    } while(1);
}

// go, gO, GO!
void main(uint32_t reset) {
	// poke the exception handlers into place
	_setvector(PEND_SV_EXC, xPortPendSVHandler);
	_setvector(SYSTICK_EXC, xPortSysTickHandler);
	_setvector(SYSCALL_EXC, vPortSVCHandler);
	// go!
    xTaskCreate(wiggle, "WiggleLEDs", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL);
	vTaskStartScheduler();
	// oops - shouldn't return from above
	_hang();
}
