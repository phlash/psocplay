/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *----------------------------------------------------------*/

// Many register defs!
#include <cydevice_trm.h>

// --@see: https://freertos.org/a00110.html --

// --CLOCKING--
// NVL specified IMO and no boot divider chain by default, hence default bus clock
// and matching CPU clock for PSoC 5LP: 48MHz as shipped from Cypress.
#define BCLK__BUS_CLK__HZ    48000000L
#define configCPU_CLOCK_HZ   ( ( unsigned long ) BCLK__BUS_CLK__HZ )
// 1kHz (1msec) system ticks
#define configTICK_RATE_HZ   ( ( TickType_t ) 1000 )
// no software timers, we have hardware and tick hooks
#define configUSE_TIMERS     0
// low power (disables ticks, waits for interrupts) if nothing to run..
#define configUSE_TICKLESS_IDLE 1

// --SCHEDULING--
// port specific task selection as we have it (faster)
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1
// pre-emptive scheduling, in case a task goes awol
#define configUSE_PREEMPTION        1
// per-tick time-slicing between equal priority tasks, avoids hogging
#define configUSE_TIME_SLICING        1
// idle hook, 'cause we're likely to have stuff to clean up in the background
#define configUSE_IDLE_HOOK         1
// tick hook, 'cause we're definitely going to be timing things in msecs
#define configUSE_TICK_HOOK         1
// 5 task priority levels - do we need this many even?
#define configMAX_PRIORITIES        ( 5 )
// 12 chars should be enough for any task name..
#define configMAX_TASK_NAME_LEN     ( 12 )
// 32bit tick counter (default)
#define configUSE_16_BIT_TICKS      0
// not going to need other idle tasks, we have hooks
#define configIDLE_SHOULD_YIELD     0
// not going to use co-routines, keeping it simple
#define configUSE_CO_ROUTINES       0
// suuport task notifications to avoid queues if possible (eg:event bits)
#define configUSE_TASK_NOTIFICATIONS 1
// profiling data is useful .. we'll fake it for now
#define configGENERATE_RUN_TIME_STATS 1
extern void vApplicationConfigStatsTimer(void);
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() vApplicationConfigStatsTimer()
extern unsigned long vApplicationGetStatsTimer(void);
#define portGET_RUN_TIME_COUNTER_VALUE() vApplicationGetStatsTimer()
// pretty printing tasks.. sadly depends on sprintf() which b0rks stuff
#define configUSE_TRACE_FACILITY    1
//#define configUSE_STATS_FORMATTING_FUNCTIONS 1

// --SYNCHRONISATION--
// mutexes are good.. recursive are safer!
#define configUSE_MUTEXES           1
#define configUSE_RECURSIVE_MUTEXES 1
// semaphores that don't count => mutexes, so..
#define configUSE_COUNTING_SEMAPHORES    1
// not going to need special queue APIs
#define configUSE_ALTERNATIVE_API   0
// 10 debuggable (named) queues should be enough for any simple app ;)
#define configQUEUE_REGISTRY_SIZE   10
// not going to pend on multiple queues, keeping it simple
#define configUSE_QUEUE_SETS        0

// --MEMORY MANAGEMENT--
// this seems a bit large: ~25 function calls depth.. could reduce after profiling?
#define configSTACK_DEPTH_TYPE        unsigned short
#define configMINIMAL_STACK_SIZE    ( ( unsigned short ) 100 )
// red-zone detection, application hook called on overflow.. record offending task and restart?
#define configCHECK_FOR_STACK_OVERFLOW   2
// heap in 32k block of non-icache data ram, kernel structures/stack in icache'd code ram
#define configTOTAL_HEAP_SIZE       ( ( size_t ) ( 32 * 1024 ) )
#define configAPPLICATION_ALLOCATED_HEAP    1
// can we do anything if the heap fills up...?
#define configUSE_MALLOC_FAILED_HOOK     1
// each task has one TLS pointer - use a TLS structure please :)
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS ( 1 )


/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet            1
#define INCLUDE_uxTaskPriorityGet           1
#define INCLUDE_vTaskDelete                 1
#define INCLUDE_vTaskCleanUpResources       0
#define INCLUDE_vTaskSuspend                1
#define INCLUDE_vTaskDelayUntil             1
#define INCLUDE_vTaskDelay                  1
#define INCLUDE_uxTaskGetStackHighWaterMark 1
#define INCLUDE_eTaskGetState               1

/**
 * Configure the number of priority bits. This is normally
 * __NVIC_PRIO_BITS but PSoC Creator beta 5 contained a larger
 * value for the priority than is implemented in the hardware so
 * set it here to what the data sheet describes.
 */
#define configPRIO_BITS       3        /* 8 priority levels */

/* The lowest priority. */
#define configKERNEL_INTERRUPT_PRIORITY     ( 7 << (8 - configPRIO_BITS) )

/* Priority 5, or 160 as only the top three bits are implemented. */
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY     ( 5 << (8 - configPRIO_BITS) )

#endif /* FREERTOS_CONFIG_H */
