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
    the FAQ page "My application does not run, what could be wwrong?".  Have you
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

/*
FreeRTOS is a market leading RTOS from Real Time Engineers Ltd. that supports
31 architectures and receives 77500 downloads a year. It is professionally
developed, strictly quality controlled, robust, supported, and free to use in
commercial products without any requirement to expose your proprietary source
code.

This simple FreeRTOS demo does not make use of any IO ports, so will execute on
any Cortex-M3 of Cortex-M4 hardware.  Look for TODO markers in the code for
locations that may require tailoring to, for example, include a manufacturer
specific header file.

This is a starter project, so only a subset of the RTOS features are
demonstrated.  Ample source comments are provided, along with web links to
relevant pages on the http://www.FreeRTOS.org site.

Here is a description of the project's functionality:

The main() Function:
main() creates the tasks and software timers described in this section, before
starting the scheduler.

The Queue Send Task:
The queue send task is implemented by the prvQueueSendTask() function.
The task uses the FreeRTOS vTaskDelayUntil() and xQueueSend() API functions to
periodically send the number 100 on a queue.  The period is set to 200ms.  See
the comments in the function for more details.
http://www.freertos.org/vtaskdelayuntil.html
http://www.freertos.org/a00117.html

The Queue Receive Task:
The queue receive task is implemented by the prvQueueReceiveTask() function.
The task uses the FreeRTOS xQueueReceive() API function to receive values from
a queue.  The values received are those sent by the queue send task.  The queue
receive task increments the ulCountOfItemsReceivedOnQueue variable each time it
receives the value 100.  Therefore, as values are sent to the queue every 200ms,
the value of ulCountOfItemsReceivedOnQueue will increase by 5 every second.
http://www.freertos.org/a00118.html

An example software timer:
A software timer is created with an auto reloading period of 1000ms.  The
timer's callback function increments the ulCountOfTimerCallbackExecutions
variable each time it is called.  Therefore the value of
ulCountOfTimerCallbackExecutions will count seconds.
http://www.freertos.org/RTOS-software-timer.html

The FreeRTOS RTOS tick hook (or callback) function:
The tick hook function executes in the context of the FreeRTOS tick interrupt.
The function 'gives' a semaphore every 500th time it executes.  The semaphore
is used to synchronise with the event semaphore task, which is described next.

The event semaphore task:
The event semaphore task uses the FreeRTOS xSemaphoreTake() API function to
wait for the semaphore that is given by the RTOS tick hook function.  The task
increments the ulCountOfReceivedSemaphores variable each time the semaphore is
received.  As the semaphore is given every 500ms (assuming a tick frequency of
1KHz), the value of ulCountOfReceivedSemaphores will increase by 2 each second.

The idle hook (or callback) function:
The idle hook function queries the amount of free FreeRTOS heap space available.
See vApplicationIdleHook().

The malloc failed and stack overflow hook (or callback) functions:
These two hook functions are provided as examples, but do not contain any
functionality.
*/

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include "stm32f4_discovery.h"
/* Kernel includes. */
#include "stm32f4xx.h"

#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/queue.h"
#include "../FreeRTOS_Source/include/semphr.h"
#include "../FreeRTOS_Source/include/task.h"
#include "../FreeRTOS_Source/include/timers.h"

#include "taskheader.h"

/*-----------------------------------------------------------*/
//#define QUEUE_LENGTH 100
//#define TASK1_LENGTH 100
//
//typedef enum task_type {PERIODIC,APERIODIC} task_type;
//
//typedef struct DD_Task {
//	TaskHandle_t t_handle;
//	enum task_type type;
//	uint32_t task_id;
//	uint32_t release_time;
//	uint32_t absolute_deadline;
//	uint32_t completion_time;
//} DD_Task;
//// Singly-linked list of DD tasks
//typedef struct DD_Task_List {
// DD_Task task;
//struct DD_Task_List *next_task;
//} DD_Task_List;

/*
 * The queue send and receive tasks as described in the comments at the top of
 * this file.
 */
//This function receives all of the information necessary to create a new dd_task struct (excluding
//the release time and completion time). The struct is packaged as a message and sent to a queue
//for the DDS to receive.
//void create_dd_task( TaskHandle_t t_handle,
//		enum task_type type,
//		uint32_t task_id,
//		uint32_t absolute_deadline);
//
////This function receives the ID of the DD-Task which has completed its execution. The ID is packaged
////as a message and sent to a queue for the DDS to receive.
//void delete_dd_task(uint32_t task_id);
//
////This function sends a message to a queue requesting the Active Task List from the DDS. Once a
////response is received from the DDS, the function returns the list.
//DD_Task_List* get_active_dd_task_list(void);
//
////This function sends a message to a queue requesting the Completed Task List from the DDS. Once
////a response is received from the DDS, the function returns the list.
//DD_Task_List* get_complete_dd_task_list(void);
//
////This function sends a message to a queue requesting the Overdue Task List from the DDS. Once a
////response is received from the DDS, the function returns the list.
//DD_Task_List* get_overdue_dd_task_list(void);
//
//static void DDS_Task( void *pvParameters );
//static void User_Defined1_Task( void *pvParameters );
//static void User_Defined2_Task( void *pvParameters );
//static void User_Defined3_Task( void *pvParameters );
//static void DDT_Generator_Task( void *pvParameters );
//static void Monitor_Task( void *pvParameters );

xQueueHandle xQueue_toDDS = 0;
xQueueHandle xQueue_fromDDS = 0;
//
//typedef enum DDS_Command {
//	CREATE,
//	DELETE,
//	GET_ACTIVE,
//	GET_COMPLETE,
//	GET_OVERDUE
//} DDS_Command;
//
//typedef struct DD_Info {
//	TaskHandle_t t_handle;
//	task_type type;
//	uint32_t task_id;
//	uint32_t absolute_deadline;
//} DD_Info;
//
//typedef struct ToDDS_Message {
//	DDS_Command msg;
//	DD_Info info;
//}ToDDS_Message;
//
//typedef enum DDS_Response{
//	ACTIVE_LIST,
//	COMPLETED_LIST,
//	OVERDUE_LIST,
//	DELETED_TASK,
//} DDS_Response;
//typedef struct FromDDS_Message {
//	DDS_Response msg;
//	DD_Task_List tasks;
//}FromDDS_Message;
/*-----------------------------------------------------------*/

#define amber_led	LED3
#define green_led	LED4
#define red_led		LED5
#define blue_led	LED6

int main(void)
{
	/* Initialize LEDs */
	STM_EVAL_LEDInit(amber_led);
	STM_EVAL_LEDInit(green_led);
	STM_EVAL_LEDInit(red_led);
	STM_EVAL_LEDInit(blue_led);

//	STM_EVAL_LEDOn(blue_led);

	/* Create the queue used by the queue send and queue receive tasks.
	http://www.freertos.org/a00116.html */
	xQueue_fromDDS = xQueueCreate( 	QUEUE_LENGTH,		/* The number of items the queue can hold. */
							sizeof( FromDDS_Message ) );	/* The size of each item the queue holds. */
	xQueue_toDDS = xQueueCreate( 	QUEUE_LENGTH,		/* The number of items the queue can hold. */
							sizeof( ToDDS_Message ) );	/* The size of each item the queue holds. */

	/* Add to the registry, for the benefit of kernel aware debugging. */
	vQueueAddToRegistry( xQueue_toDDS, "MainQueue");

//	xTaskCreate( User_Defined1_Task, "UserDef1", configMINIMAL_STACK_SIZE, NULL, 0, NULL);
//	xTaskCreate( User_Defined2_Task, "UserDef2", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
//	xTaskCreate( User_Defined3_Task, "UserDef3", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
//
	xTaskCreate( DDS_Task, "DDS", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
	xTaskCreate( DDT_Generator_Task, "DDT Generator", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
	xTaskCreate( Monitor_Task, "Monitor", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

	/* Start the tasks and timer running. */
	vTaskStartScheduler();

	return 0;
}

/*-----------------------------------------------------------*/

void DDS_Task( void *pvParameters )
{
	DD_Task_List active_tasks;
	active_tasks.task.t_handle = (void*)NULL;
	DD_Task_List completed_tasks;
	DD_Task_List overdue_tasks;
	ToDDS_Message msg;
	while(1)
	{
		xQueueReceive( xQueue_toDDS, &msg, 1000);
		switch (msg.msg) {
		case DELETE:
//			vTaskPrioritySet(msg.info.t_handle, 1);
			// TODO lookup in active tasks and vTaskSuspend( )
			if (active_tasks.task.t_handle == NULL) {
				break;
			}
//			vTaskSuspend(active_tasks.task.t_handle);
//			vTaskDelete(active_tasks.task.t_handle);


			// mark as completed
			completed_tasks.task.t_handle = msg.info.t_handle;
			completed_tasks.task.type = msg.info.type;
			completed_tasks.task.absolute_deadline = msg.info.absolute_deadline;
			completed_tasks.task.task_id = msg.info.task_id;
			completed_tasks.task.task_id = msg.info.task_id;

			// send tasks & specific task
			// *task list & *taskhandle_t
			FromDDS_Message tmp;
			tmp.msg = DELETED_TASK;
			tmp.tasks.task.task_id= active_tasks.task.task_id;
			tmp.tasks.task.t_handle= active_tasks.task.t_handle;
			// delete active task
			// todo actually look this up
			active_tasks.task.t_handle = (void*)NULL;

			xQueueSend(xQueue_fromDDS, &tmp, 1000);
			STM_EVAL_LEDToggle(red_led);
			break;
		case CREATE:
			STM_EVAL_LEDToggle(amber_led);
			active_tasks.task.t_handle = msg.info.t_handle;
			active_tasks.task.type = msg.info.type;
			active_tasks.task.absolute_deadline = msg.info.absolute_deadline;
			active_tasks.task.task_id = msg.info.task_id;
			vTaskPrioritySet(msg.info.t_handle, 2);
			vTaskResume(msg.info.t_handle);
			break;
		default:
			STM_EVAL_LEDToggle(green_led);
			break;
		}
		// push active tasks to active task queue
		// push completed tasks to complete task queue
		// push overdue tasks to overdue task queue

	}
}
//static void count_task( void *pvParameters ){
//	while  (1) {
//		vTaskDelay(1000);
//		STM_EVAL_LEDToggle(amber_led);
//	}
//}

void User_Defined1_Task( void *pvParameters )
{
	while(1)
	{
		STM_EVAL_LEDToggle(blue_led);
		delete_dd_task(1);
		vTaskDelay(1000);
	}
}

void User_Defined2_Task( void *pvParameters )
{
	while(1)
	{
	}
}

void User_Defined3_Task( void *pvParameters )
{
	while(1)
	{
	}
}

void DDT_Generator_Task( void *pvParameters )
{
	TaskHandle_t task1;
	xTaskCreate( User_Defined1_Task, "UserDef1", configMINIMAL_STACK_SIZE, NULL, 0, &task1);

	while(1)
	{
		vTaskSuspend(task1);

		create_dd_task(task1,
				PERIODIC,
				1,
				100);
		vTaskDelay(1000);

	}
}
void Monitor_Task( void *pvParameters )
{
	while(1)
	{
		vTaskDelay(100);

	}
}

//This function receives all of the information necessary to create a new dd_task struct (excluding
//the release time and completion time). The struct is packaged as a message and sent to a queue
//for the DDS to receive.
void create_dd_task(
		TaskHandle_t t_handle,
		task_type type,
		uint32_t task_id,
		uint32_t absolute_deadline)
{
	DD_Info task_info = {
		t_handle, //TaskHandle_t t_handle
		type, //task_type type;
		task_id,
		absolute_deadline, //uint32_t absolute_deadline;
	};
	ToDDS_Message msg = {
		CREATE,
		task_info
	};
	xQueueSend(xQueue_toDDS, &msg, 0);
}

//This function receives the ID of the DD-Task which has completed its execution. The ID is packaged
//as a message and sent to a queue for the DDS to receive.
void delete_dd_task(uint32_t task_id)
{
	DD_Info task_info = {
		NULL, //TaskHandle_t t_handle
		0, //task_type type;
		task_id,
		0, //uint32_t absolute_deadline;
	};
	ToDDS_Message msg = {
		DELETE,
		task_info
	};
	FromDDS_Message rsp;
	xQueueSend(xQueue_toDDS, &msg, 1000);
	xQueueReceive(xQueue_fromDDS, &rsp, 1000 );
	for (;;) {
		if (rsp.msg == DELETED_TASK && rsp.tasks.task.task_id == task_id) {
			vTaskSuspend(rsp.tasks.task.t_handle);
			return;
		}else {
			xQueueSend(xQueue_fromDDS, &rsp, 1000 );
			xQueueReceive(xQueue_fromDDS, &rsp, 1000);
		}
	}
}

//This function sends a message to a queue requesting the Active Task List from the DDS. Once a
//response is received from the DDS, the function returns the list.
DD_Task_List* get_active_dd_task_list(void)
{

}

//This function sends a message to a queue requesting the Completed Task List from the DDS. Once
//a response is received from the DDS, the function returns the list.
DD_Task_List* get_complete_dd_task_list(void)
{

}

//This function sends a message to a queue requesting the Overdue Task List from the DDS. Once a
//response is received from the DDS, the function returns the list.
DD_Task_List* get_overdue_dd_task_list(void)
{

}

/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* The malloc failed hook is enabled by setting
	configUSE_MALLOC_FAILED_HOOK to 1 in FreeRTOSConfig.h.

	Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software 
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected.  pxCurrentTCB can be
	inspected in the debugger if the task name passed into this function is
	corrupt. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
volatile size_t xFreeStackSpace;

	/* The idle task hook is enabled by setting configUSE_IDLE_HOOK to 1 in
	FreeRTOSConfig.h.

	This function is called on each cycle of the idle task.  In this case it
	does nothing useful, other than report the amount of FreeRTOS heap that
	remains unallocated. */
	xFreeStackSpace = xPortGetFreeHeapSize();

	if( xFreeStackSpace > 100 )
	{
		/* By now, the kernel has allocated everything it is going to, so
		if there is a lot of heap remaining unallocated then
		the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		reduced accordingly. */
	}
}

