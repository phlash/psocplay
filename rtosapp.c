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
static uint32_t idleCnt;
void vApplicationIdleHook(void) { ++idleCnt; }
// sys tick hook
static volatile int hookSaysNo;
void vApplicationTickHook(void) {
	static TickType_t last;
	static uint32_t calls;
	TickType_t now=xTaskGetTickCount();
	++calls;
	// toggle wiggling every 5 secs, for 5secs
	if (now>(last+5000)) {
		last = now;
		hookSaysNo = ~hookSaysNo;
	}
}

// stats timer (high resolution timer)
void vApplicationConfigStatsTimer(void) {}
unsigned long vApplicationGetStatsTimer(void) {
	// use systick counter value (@CPU clock rate), critical section to ensure atomicity
	taskENTER_CRITICAL();
	uint32_t systick = *(uint32_t*)(CYREG_NVIC_SYSTICK_CURRENT);
	TickType_t ticks = xTaskGetTickCount();
	taskEXIT_CRITICAL();
	uint32_t sysload = *(uint32_t*)(CYREG_NVIC_SYSTICK_RELOAD);
	return (ticks*sysload)+(sysload-systick);
}

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
