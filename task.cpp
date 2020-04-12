#include <cstdio>
#include <string>
#include "sys.h"
#include "rtos_api.h"

bool checkIfNextExists(size_t taskNum) {
    return TaskQueue[taskNum].next != -1;
}

void InsertBefore(size_t existingTaskNum, size_t insertTaskNum, size_t *head) {
    if (checkIfNextExists(existingTaskNum)) {
        return;
    }
    if (insertTaskNum == *head) {
        *head = existingTaskNum;
    }
    TaskQueue[existingTaskNum].next = insertTaskNum;
    TaskQueue[existingTaskNum].prev = TaskQueue[insertTaskNum].prev;
    TaskQueue[TaskQueue[insertTaskNum].prev].next = existingTaskNum;
    TaskQueue[insertTaskNum].prev = existingTaskNum;
}

void InsertAfter(size_t existingTaskNum, size_t insertTaskNum) {
    if (checkIfNextExists(existingTaskNum)) {
        return;
    }
    TaskQueue[existingTaskNum].next = TaskQueue[insertTaskNum].next;
    TaskQueue[existingTaskNum].prev = insertTaskNum;
    TaskQueue[TaskQueue[insertTaskNum].next].prev = existingTaskNum;
    TaskQueue[insertTaskNum].next = existingTaskNum;
}

void RemoveTask(size_t existingTaskNum, size_t *head) {
    if (checkIfNextExists(existingTaskNum)) {
        return;
    }
    if (*head == existingTaskNum) {
        if (TaskQueue[existingTaskNum].next == existingTaskNum) {
            *head = -1;
        } else {
            *head = TaskQueue[existingTaskNum].next;
        }
    }
    TaskQueue[TaskQueue[existingTaskNum].prev].next = TaskQueue[existingTaskNum].next;
    TaskQueue[TaskQueue[existingTaskNum].next].prev = TaskQueue[existingTaskNum].prev;
    TaskQueue[existingTaskNum].next = -1;
    TaskQueue[existingTaskNum].prev = -1;
}

void ActivateTask(TTaskCall entry, size_t priority, const std::string &name) {
    size_t occupy;
    std::cout << "Task [" << name << "] activation" << std::endl;
    occupy = FreeTask;

    RemoveTask(occupy, &FreeTask);
    TaskQueue[occupy].priority = priority;
    TaskQueue[occupy].ceiling_priority = priority;
    TaskQueue[occupy].name = name;
    TaskQueue[occupy].entry = entry;
    TaskQueue[occupy].switch_count = 0;
    TaskQueue[occupy].task_state = TTaskState::TASK_READY;
    TaskCount++;

    std::cout << "End of task [" << name << "] activation" << std::endl;
    Schedule(occupy);
}

void TerminateTask() {
    size_t task;
    TaskCount--;
    task = TaskInProcess;

    std::cout << "Task [" << TaskQueue[task].name << "] termination started" << std::endl;
    TaskQueue[task].task_state = TTaskState::TASK_SUSPENDED;
    RemoveTask(task, &TaskHead);
    InsertBefore(task, FreeTask, &FreeTask);

    std::cout << "End of task [" << TaskQueue[task].name << "] termination" << std::endl;
    if (TaskCount == 0) {
        longjmp(MainContext, 1);
    }
    Dispatch();
}

void Schedule(size_t task) {
    size_t cur, priority;
    if (task <= sizeof(TaskQueue) / sizeof(TaskQueue[0])
        && TaskQueue[task].task_state == TTaskState::TASK_SUSPENDED) {
        return;
    }

    std::cout << "Started scheduling of the [" << TaskQueue[task].name << "] task" << std::endl;
    if (TaskHead == -1) {
        TaskHead = task;
        TaskQueue[task].next = TaskQueue[task].prev = task;
    } else if (TaskCount > 1) {
        cur = TaskHead;
        if (cur == task) {
            cur = TaskQueue[cur].next;
        }
        priority = TaskQueue[task].ceiling_priority;
        RemoveTask(task, &TaskHead);
        while (TaskQueue[cur].ceiling_priority <= priority) {
            cur = TaskQueue[cur].next;
            if (cur == TaskHead) {
                break;
            }
        }
        if (priority >= TaskQueue[TaskHead].ceiling_priority && cur == TaskHead) {
            InsertAfter(task, TaskQueue[TaskHead].prev);
        } else {
            InsertBefore(task, cur, &TaskHead);
        }
    }
    std::cout << "End of task [" << TaskQueue[task].name << "] scheduling" << std::endl;
}

void TaskSwitch(size_t nextTask) {
    if (nextTask == -1) {
        return;
    }

    TaskQueue[nextTask].task_state = TTaskState::TASK_RUNNING;
    TaskInProcess = nextTask;
    TaskQueue[nextTask].switch_count++;

    if (TaskQueue[nextTask].switch_count == 1) {
        longjmp(InitStacks[nextTask], 1);
    } else {
        longjmp(TaskQueue[nextTask].context, 1);
    }
}

void Dispatch() {
    if (TaskInProcess != -1) {
        std::cout << "Started dispatch of the task [" << TaskQueue[TaskInProcess].name << "]" << std::endl;
    }
    if (TaskInProcess != -1 && TaskQueue[TaskInProcess].task_state == TTaskState::TASK_RUNNING) {
        TaskQueue[TaskInProcess].task_state = TTaskState::TASK_READY;
    }
    size_t cur = TaskHead;
    while (TaskCount) {
        if (TaskQueue[cur].task_state == TTaskState::TASK_READY) {
            std::cout << "Ended the dispatch of the task [" << TaskQueue[cur].name << "]" << std::endl;
            if (TaskInProcess == -1
                || !setjmp(TaskQueue[TaskInProcess].context)
                || TaskQueue[TaskInProcess].task_state == TTaskState::TASK_SUSPENDED) {
                TaskSwitch(cur);
            }
            break;
        }
        cur = TaskQueue[cur].next;
        if (cur == TaskHead) {
            std::cout << "No ready tasks at the moment" << std::endl;
            longjmp(MainContext, 1);
        }
    }
    Schedule(cur);
}
