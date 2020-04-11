#pragma once

#include "sys.h"

#define DeclareTask(TaskID,priority)\
TASK(TaskID); \
enum {TaskID##prior=priority}
#define TASK(TaskID) void TaskID(void)
typedef void TTaskCall(void);
void ActivateTask(TTaskCall entry, size_t priority, std::string name);
void TerminateTask(void);
size_t StartOS( const TTaskCall & entry, size_t priority, std::string name);
void ShutdownOS();
void InitRes(size_t ResNum, std::string name);
void PIP_GetRes(size_t ResNum);
void PIP_ReleaseRes(size_t ResNum);
#define DeclareSysEvent(ID) \
const size_t Event_##ID = (ID)*(ID);
void SetSysEvent(const TEventMask & mask);
void GetSysEvent(TEventMask * mask);
void WaitSysEvent(const TEventMask & mask);