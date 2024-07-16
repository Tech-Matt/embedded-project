#ifndef SCHED_H_
#define SCHED_H_

#include <stdint.h>
#include <ti/grlib/grlib.h>

#define MAX_TASKS 10
#define STACK_SIZE MAX_TASKS

typedef enum {
    TASK_RUNNING,
    TASK_READY,
    TASK_BLOCKED,
    TASK_SUSPENDED
} TaskState;

/* This struct simulate possible data of a task/process that needs to be saved into the stack when the scheduler decides to put it
 * in ready/blocked state.
 */
typedef struct {
    uint32_t pid;
    uint32_t counter; // example field to save task specific data
    uint32_t priority;
} TaskData;

typedef struct {
    TaskState state;         /* Current state of the task */
    void (*taskFunc)(void);  /* Pointer to the task function */
    TaskData *stackPointer;  /* Pointer to the location of the task's data in the stack */
} TaskControlBlock;

extern TaskControlBlock tasks[MAX_TASKS];
extern TaskData stack[STACK_SIZE];
extern int currentTask;

void initTasks(void);
int createTask(void (*taskFunc)(void), uint32_t taskId, uint32_t pid, uint32_t priority);
void scheduler(Graphics_Context *context);

#endif /* SCHED_H_ */
