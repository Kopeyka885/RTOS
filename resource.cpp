#include "sys.h"
#include "rtos_api.h"
#include <stdio.h>
#include <iostream>
void InitRes(size_t ResNum, std::string name)
{
	std::cout << "InitRes " << name << std::endl;
	ResourceQueue[ResNum].name = name;
	ResourceQueue[ResNum].task = -1;
}
void PIP_GetRes(size_t ResNum)
{
	std::cout <<"PIP_GetRes %s" << ResourceQueue[ResNum].name << std::endl;
	while (ResourceQueue[ResNum].task != -1)
	{
		size_t rtask = ResourceQueue[ResNum].task;
		std::cout <<"Resource is blocked by %s" << TaskQueue[rtask].name << std::endl;
		if (TaskQueue[rtask].ceiling_priority >=
			TaskQueue[RunningTask].ceiling_priority)
		{
			std::cout <<"Raise priority from %i to %i "
				<< " " << TaskQueue[rtask].ceiling_priority
				 << " " << (TaskQueue[RunningTask].ceiling_priority - 1);
			TaskQueue[rtask].ceiling_priority = TaskQueue[RunningTask].ceiling_priority - 1;
			Schedule(rtask);
		}
		Dispatch();
	}
	ResourceQueue[ResNum].task = RunningTask;
	std::cout <<"End of PIP_GetRes %s" << ResourceQueue[ResNum].name << std::endl;
}
void PIP_ReleaseRes(size_t ResNum)
{
	std::cout <<"PIP_ReleaseRes %s " << ResourceQueue[ResNum].name << std::endl;
	size_t rtask = ResourceQueue[ResNum].task;
	ResourceQueue[ResNum].task = -1;
	if (TaskQueue[rtask].ceiling_priority != TaskQueue[rtask].priority)
	{
		std::cout <<"Reset prioritet for %s from %i to %i" <<
			" " <<TaskQueue[rtask].name << " " <<
			TaskQueue[rtask].ceiling_priority << " " <<
			(TaskQueue[rtask].priority) << std::endl;
		TaskQueue[rtask].ceiling_priority = TaskQueue[rtask].priority;
		Schedule(rtask);
	}
	
	std::cout <<"End of PIP_ReleaseRes %s " << ResourceQueue[ResNum].name << std::endl;
}