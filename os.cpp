#include <stdio.h>
#include "sys.h"
#include "rtos_api.h"
void InitializeStacks(size_t numStack)
{
	/*char cushionSpace[100000];
	cushionSpace[99999] = 1; */
	if (!setjmp(InitStacks[numStack]))
	{
		if (numStack <= MAX_TASK)
			InitializeStacks(numStack + 1);
	}
	else
	{
		TaskQueue[RunningTask].entry();
	}
}
size_t StartOS(const TTaskCall & entry, size_t priority, std::string name)
{
	size_t i;
	InitializeStacks(0);
	RunningTask = TaskHead = -1;
	TaskCount = 0;
	FreeTask = 0;
	WorkingEvents = 0;
	std::cout << "StartOS! " << std::endl;
	for (i = 0; i < MAX_TASK; i++)
	{
		TaskQueue[i].next = i + 1;
		TaskQueue[i].prev = i - 1;
		TaskQueue[i].task_state = TASK_SUSPENDED;
		TaskQueue[i].switch_count = 0;
		TaskQueue[i].waiting_events = 0;
	}
	TaskQueue[MAX_TASK - 1].next = 0;
	TaskQueue[0].prev = MAX_TASK - 1;
	if (!setjmp(MainContext))
	{
		ActivateTask(entry, priority, name);
		Dispatch();
	}
	return 0;
}

void ShutdownOS()
{
	std::cout << "ShutdownOS !" << std::endl;
}