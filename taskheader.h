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

#define QUEUE_LENGTH 100
#define TASK1_LENGTH 100

typedef enum task_type {PERIODIC,APERIODIC} task_type;

typedef enum DDS_Command {
	CREATE,
	DELETE,
	GET_ACTIVE,
	GET_COMPLETE,
	GET_OVERDUE
} DDS_Command;

typedef enum DDS_Response{
	ACTIVE_LIST,
	COMPLETED_LIST,
	OVERDUE_LIST,
	DELETED_TASK,
} DDS_Response;

typedef struct DD_Task {
	TaskHandle_t t_handle;
	enum task_type type;
	uint32_t task_id;
	uint32_t release_time;
	uint32_t absolute_deadline;
	uint32_t completion_time;
} DD_Task;

// Singly-linked list of DD tasks
typedef struct DD_Task_List {
	DD_Task task;
	struct DD_Task_List *next_task;
} DD_Task_List;

typedef struct DD_Info {
	TaskHandle_t t_handle;
	task_type type;
	uint32_t task_id;
	uint32_t absolute_deadline;
} DD_Info;

typedef struct ToDDS_Message {
	DDS_Command msg;
	DD_Info info;
}ToDDS_Message;


typedef struct FromDDS_Message {
	DDS_Response msg;
	DD_Task_List tasks;
}FromDDS_Message;

/*
 * The queue send and receive tasks as described in the comments at the top of
 * this file.
 */
//This function receives all of the information necessary to create a new dd_task struct (excluding
//the release time and completion time). The struct is packaged as a message and sent to a queue
//for the DDS to receive.
void create_dd_task( TaskHandle_t t_handle,
		enum task_type type,
		uint32_t task_id,
		uint32_t absolute_deadline);

//This function receives the ID of the DD-Task which has completed its execution. The ID is packaged
//as a message and sent to a queue for the DDS to receive.
void delete_dd_task(uint32_t task_id);

//This function sends a message to a queue requesting the Active Task List from the DDS. Once a
//response is received from the DDS, the function returns the list.
DD_Task_List* get_active_dd_task_list(void);

//This function sends a message to a queue requesting the Completed Task List from the DDS. Once
//a response is received from the DDS, the function returns the list.
DD_Task_List* get_complete_dd_task_list(void);

//This function sends a message to a queue requesting the Overdue Task List from the DDS. Once a
//response is received from the DDS, the function returns the list.
DD_Task_List* get_overdue_dd_task_list(void);

static void DDS_Task( void *pvParameters );
static void User_Defined1_Task( void *pvParameters );
static void User_Defined2_Task( void *pvParameters );
static void User_Defined3_Task( void *pvParameters );
static void DDT_Generator_Task( void *pvParameters );
static void Monitor_Task( void *pvParameters );
