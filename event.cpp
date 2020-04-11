#include <stdio.h>
#include "sys.h"
#include "rtos_api.h"
void SetSysEvent(const TEventMask & mask)
{
	std::cout << "SetSysEvent " << mask << std::endl;
	WorkingEvents |= mask;
	for (size_t i = 0; i < MAX_TASK; i++)
	{
		if (TaskQueue[i].task_state == TASK_WAITING &&
			(WorkingEvents & TaskQueue[i].waiting_events))
		{
			TaskQueue[i].waiting_events &= !mask;
			TaskQueue[i].task_state = TASK_READY; 
			std::cout << "Task is ready" << std::endl; 
			std::cout << TaskQueue[i].name << std::endl;
		}
	}
	WorkingEvents &= !mask;
	std::cout << "End of SetSysEvent "<< mask << std::endl;
}
void GetSysEvent(TEventMask * mask)
{
	*mask = WorkingEvents;
}
void WaitSysEvent(const TEventMask & mask)
{
	std::cout << "WaitSysEvent " << mask << std::endl;
	TaskQueue[RunningTask].waiting_events = mask;
	if ((WorkingEvents & mask) == 0)
	{//ждемс
		TaskQueue[RunningTask].task_state = TASK_WAITING;
		Dispatch();
	}
	std::cout << "End of WaitSysEvent "<< mask << std::endl;
}