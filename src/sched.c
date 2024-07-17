#include <sched.h>
#include <stdio.h>
#include <string.h>

#include "msp.h"
#include "utils.h"
#include "LcdDriver/Crystalfontz128x128_ST7735.h"

TaskControlBlock tasks[MAX_TASKS];
TaskData stack[STACK_SIZE];
int currentTask = -1;
int stackTop = 0; // Points to the next free slot in the stack

void initTasks(void) {
    int i;
    for (i = 0; i < MAX_TASKS; i++) {
        tasks[i].state = TASK_SUSPENDED;
        tasks[i].taskFunc = NULL;
        tasks[i].stackPointer = NULL;
    }
}

int createTask(void (*taskFunc)(void), uint32_t taskId, uint32_t pid, uint32_t priority) {
    if (taskId >= MAX_TASKS) {
        return -1; // Invalid task ID
    }
    if (tasks[taskId].taskFunc != NULL) {
        return -1; // Task slot already in use
    }
    if (stackTop >= STACK_SIZE) {
        return -1; // Stack overflow
    }

    tasks[taskId].taskFunc = taskFunc;
    tasks[taskId].state = TASK_READY;
    tasks[taskId].stackPointer = &stack[stackTop];

    // Initialize TaskData
    tasks[taskId].stackPointer->pid = pid;
    tasks[taskId].stackPointer->counter = 0;  // Example initial value
    tasks[taskId].stackPointer->priority = priority;

    stackTop++;

    return 0; // Task created successfully
}
// Round-Robin scheduler (no preemption)
char log[50];
void scheduler(Graphics_Context *context) {

    // ENTERING CRITICAL SECTION, DISABLING SYSTICK
    SysTick_disableInterrupt();

    currentTask = (currentTask + 1) % MAX_TASKS;
    while (tasks[currentTask].taskFunc == NULL) { // Careful, possible infinite loop
        currentTask = (currentTask + 1) % MAX_TASKS;
    }
    // Log and execute task
    sprintf(log, "Task %d executing", currentTask);
    tasks[currentTask].taskFunc();
    logToLCD(context, log);

    // Delete log
    strcpy(log, "");

    // EXITING CRITICAL SECTION, ENABLING SYSTICK
    SysTick_enableInterrupt();
}


