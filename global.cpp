#include "sys.h"

TTask TaskQueue[MAX_TASK];
TResource ResourceQueue[MAX_RES];
jmp_buf InitStacks[MAX_TASK];
jmp_buf MainContext;
size_t TaskInProcess;
size_t TaskHead;
size_t TaskCount;
size_t FreeTask;
TEventMask EventsInProcess;
