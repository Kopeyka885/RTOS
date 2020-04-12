#include <cstdio>
#include "sys.h"
#include "rtos_api.h"

void InitializeStacks(size_t stackNum) {
    if (!setjmp(InitStacks[stackNum])) {
        if (stackNum <= MAX_TASK) {
            InitializeStacks(stackNum + 1);
        }
    } else {
        TaskQueue[TaskInProcess].entry();
    }
}

void InitializeTask(TTask &task, size_t next, size_t prev) {
    task.next = next;
    task.prev = prev;
    task.task_state = TTaskState::TASK_SUSPENDED;
    task.switch_count = 0;
    task.waiting_events = 0;
}

void StartOS(TTaskCall &task, size_t priority, const std::string &name) {
    size_t i;
    InitializeStacks(0);
    TaskInProcess = TaskHead = -1;
    TaskCount = FreeTask = EventsInProcess = 0;

    std::cout << "OS initialized..." << std::endl;
    for (i = 0; i < MAX_TASK; i++) {
        InitializeTask(TaskQueue[i], i + 1, i - 1);
    }
    TaskQueue[MAX_TASK - 1].next = 0;
    TaskQueue[0].prev = MAX_TASK - 1;

    if (!setjmp(MainContext)) {
        ActivateTask(task, priority, name);
        Dispatch();
    }
}

void ShutdownOS() {
    std::cout << "OS shutdown..." << std::endl;
}