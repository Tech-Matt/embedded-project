#include "task.h"
#include "msp.h"

TaskControlBlock tasks[MAX_TASKS];
int currentTask = -1;

void initTasks(void) {
    int i = 0;
    for (i = 0; i < MAX_TASKS; i++) {
        tasks[i].taskFunc = 0;
        tasks[i].stackPointer = 0;
    }
}

void createTask(void (*taskFunc)(void), int taskId) {
    tasks[taskId].taskFunc = taskFunc;
    tasks[taskId].stackPointer = &tasks[taskId].stack[STACK_SIZE - 1];
}

// Round-Robin scheduler
void scheduler(void) {
    currentTask = (currentTask + 1) % MAX_TASKS;
    while (tasks[currentTask].taskFunc == 0) { // Careful, possible infinite loop
        currentTask = (currentTask + 1) % MAX_TASKS;
    }
    tasks[currentTask].taskFunc();
}


