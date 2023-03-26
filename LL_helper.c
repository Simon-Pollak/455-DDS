#include "taskheader.h"

DD_Task_List* createNode(DD_Task* task){
	DD_Task_List* l = (DD_Task_List*)pvPortMalloc(sizeof(DD_Task));
	l->task = *task;
	l->next_task = NULL;
	return l;
}
