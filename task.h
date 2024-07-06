#ifndef TASK_H_
#define TASK_H_

#include <stdint.h>

#define MAX_TASKS 3
#define STACK_SIZE 1024

typedef struct {
    void (*taskFunc)(void);
    uint8_t *stackPointer;
    uint8_t stack[STACK_SIZE];
} TaskControlBlock;

extern TaskControlBlock tasks[MAX_TASKS];
extern int currentTask;

void initTasks(void);
void createTask(void (*taskFunc)(void), int taskId);
void scheduler(void);

#endif /* TASK_H_ */
