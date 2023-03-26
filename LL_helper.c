#include "taskheader.h"

DD_Task_List* createNode(DD_Task task){
	DD_Task_List* l = (DD_Task_List*)pvPortMalloc(sizeof(DD_Task_List));
	l->task = task;
	l->next_task = NULL;
	return l;
}
//This function adds new nodes in the appropriate location based on absolute deadline
//so there is no need to sort during runtime.
void addTask( DD_Task_List** head, DD_Task task)
{
	DD_Task_List* newnode = createNode(task);
	//make first node if list is empty
	if(*head == NULL)
	{
		*head = newnode;
		return;
	}
	DD_Task_List* ref = *head;
	//add task at front of list if the new deadline is earlier
	if(newnode->task.absolute_deadline < ref->task.absolute_deadline)
	{
		newnode->next_task = ref;
		return;
	}
	//if deadline is before new task to be added, shift reference
	while(ref->next_task->task.absolute_deadline <= newnode->task.absolute_deadline)
	{
		ref = ref->next_task;
	}
	//insert the new node after current reference
	newnode->next_task = ref->next_task;
	ref->next_task = newnode;
}
//remove task based on task ID
void removeTask( DD_Task_List** head, uint32_t task_id)
{

}
