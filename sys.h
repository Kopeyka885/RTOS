#pragma once

#include "defs.h"
#include <csetjmp>
#include <iostream>
#include <memory>
#include <string>
enum TTaskState {
	TASK_RUNNING = 0, TASK_READY = 1, TASK_SUSPENDED = 2, TASK_WAITING = 3
};
typedef size_t TEventMask;
typedef struct Type_Task
{
	size_t next, prev;
	size_t priority; // ��� ������ �����, ��� ������ ���������
	size_t ceiling_priority;
	void(*entry)(void);
	std::string name;
	TTaskState task_state;
	size_t switch_count;
	jmp_buf context;
	TEventMask waiting_events;
} TTask;
typedef struct Type_resource
{
	size_t task;
	//size_t priority;
	std::string name;
} TResource;
extern jmp_buf InitStacks[MAX_TASK];
extern TTask TaskQueue[MAX_TASK];
extern TResource ResourceQueue[MAX_RES];
extern size_t RunningTask; 
extern size_t TaskHead; //������ ������ � ������, ����������� ��������� �� ����������
extern size_t TaskCount;
extern size_t FreeTask;
extern TEventMask WorkingEvents;//����������� �������
extern jmp_buf MainContext; //������� ��������
void Schedule(size_t task, size_t dont_show = 0);
void Dispatch();