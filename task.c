#include "task.h"
#include "msp.h"

TaskControlBlock tasks[MAX_TASKS];
int currentTask = -1;

void initTasks(void) {
    for (int i = 0; i < MAX_TASKS; i++) {
        tasks[i].taskFunc = 0;
        tasks[i].stackPointer = 0;
    }
}

void createTask(void (*taskFunc)(void), int taskId) {
    tasks[taskId].taskFunc = taskFunc;
    tasks[taskId].stackPointer = &tasks[taskId].stack[STACK_SIZE - 1];
}

void scheduler(void) {
    currentTask = (currentTask + 1) % MAX_TASKS;
    while (tasks[currentTask].taskFunc == 0) {
        currentTask = (currentTask + 1) % MAX_TASKS;
    }
    tasks[currentTask].taskFunc();
}


