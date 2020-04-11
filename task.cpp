#include <stdio.h>
#include <string>
#include "sys.h"
#include "rtos_api.h"

void InsertTaskBefore(size_t task, size_t item, size_t *head)
{
	if (TaskQueue[task].next != -1)
		return;
	if (item == *head)
		*head = task;
	TaskQueue[task].next = item;
	TaskQueue[task].prev = TaskQueue[item].prev;
	TaskQueue[TaskQueue[item].prev].next = task;
	TaskQueue[item].prev = task;
}
void InsertTaskAfter(size_t task, size_t item)
{
	if (TaskQueue[task].next != -1)
		return;
	TaskQueue[task].next = TaskQueue[item].next;
	TaskQueue[task].prev = item;
	TaskQueue[TaskQueue[item].next].prev = task;
	TaskQueue[item].next = task;
}
void RemoveTask(size_t task, size_t *head)
{
	if (TaskQueue[task].next == -1)
		return;
	if (*head == task)
	{
		if (TaskQueue[task].next == task)
			*head = -1;
		else
			*head = TaskQueue[task].next;
	}
	TaskQueue[TaskQueue[task].prev].next = TaskQueue[task].next;
	TaskQueue[TaskQueue[task].next].prev = TaskQueue[task].prev;
	TaskQueue[task].next = -1;
	TaskQueue[task].prev = -1;
}
void ActivateTask(TTaskCall entry, size_t priority, std::string name)
{
	size_t task, occupy;
	std::cout <<"ActivateTask " << name << std::endl;
	task = TaskHead;
	occupy = FreeTask;
	
	RemoveTask(occupy, &FreeTask);
	TaskQueue[occupy].priority = priority;
	TaskQueue[occupy].ceiling_priority = priority;
	TaskQueue[occupy].name = name;
	TaskQueue[occupy].entry = entry;
	TaskQueue[occupy].switch_count = 0;
	TaskQueue[occupy].task_state = TASK_READY;
	TaskCount++;
	std::cout <<"End of ActivateTask " << name << std::endl;
	Schedule(occupy);
}
void TerminateTask(void)
{
	size_t task;
	TaskCount--;
	task = RunningTask;
	std::cout <<"TerminateTask "<< TaskQueue[task].name << std::endl;
	TaskQueue[task].task_state = TASK_SUSPENDED;
	RemoveTask(task, &TaskHead);
	InsertTaskBefore(task, FreeTask, &FreeTask);
	std::cout <<"End of TerminateTask " << TaskQueue[task].name << std::endl;
	if (TaskCount == 0)
		longjmp(MainContext, 1);
	Dispatch();
}
void Schedule(size_t task, size_t dont_show)
{
	size_t cur;
	size_t priority;
	if (task <= sizeof(TaskQueue) / sizeof(TaskQueue[0]) && TaskQueue[task].task_state == TASK_SUSPENDED)
		return;
	if (!dont_show)
		std::cout <<"Schedule " << TaskQueue[task].name << std::endl;
	if (TaskHead == -1)
	{
		TaskHead = task;
		TaskQueue[task].next = task;
		TaskQueue[task].prev = task;
	}
	else if (TaskCount > 1)
	{
		cur = TaskHead;
		if (cur == task)
		
			cur = TaskQueue[cur].next;
		priority = TaskQueue[task].ceiling_priority;
		RemoveTask(task, &TaskHead);
		while (TaskQueue[cur].ceiling_priority <= priority)
		{
			cur = TaskQueue[cur].next;
			if (cur == TaskHead)
				break;
		}
		if (priority >= TaskQueue[TaskHead].ceiling_priority && cur == TaskHead)
			InsertTaskAfter(task, TaskQueue[TaskHead].prev);
		else
			InsertTaskBefore(task, cur, &TaskHead);
	}
	if (!dont_show)
		std::cout <<"End of Schedule " << TaskQueue[task].name << std::endl;
}
void TaskSwitch(size_t nextTask)
{
	if (nextTask == -1)
		return;
	TaskQueue[nextTask].task_state = TASK_RUNNING;
	RunningTask = nextTask;
	TaskQueue[nextTask].switch_count++;
	if (TaskQueue[nextTask].switch_count == 1)
		longjmp(InitStacks[nextTask], 1);
	else
		longjmp(TaskQueue[nextTask].context, 1);
}
void Dispatch()
{
	if (RunningTask != -1)
		std::cout <<"Dispatch - " << TaskQueue[RunningTask].name << std::endl;
	else
		std::cout <<"Dispatch " << std::endl;
	if (RunningTask != -1 && TaskQueue[RunningTask].task_state == TASK_RUNNING)
		TaskQueue[RunningTask].task_state = TASK_READY;
	size_t cur = TaskHead;
	while (TaskCount)
	{
		if (TaskQueue[cur].task_state == TASK_READY)
		{
			std::cout <<"End of Dispatch - " << TaskQueue[cur].name << std::endl;
			if (RunningTask == -1 || TaskQueue[RunningTask].task_state ==
				TASK_SUSPENDED)
				TaskSwitch(cur);
			else if (!setjmp(TaskQueue[RunningTask].context))
				TaskSwitch(cur);
			break;
		}
		cur = TaskQueue[cur].next;
		if (cur == TaskHead)
		{
			std::cout <<"Dont have ready tasks " << std::endl;
			longjmp(MainContext, 1);
		}
	}
	Schedule(cur, 1);
}