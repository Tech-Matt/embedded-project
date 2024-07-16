#ifndef SCHED_H_
#define SCHED_H_

#include <stdint.h>
#include <ti/grlib/grlib.h>


#define STACK_SIZE 1024

typedef enum {
    TASK_RUNNING,
    TASK_READY,
    TASK_BLOCKED,
    TASK_SUSPENDED
} TaskState;

typedef struct {
    uint32_t *stackPointer; /* Points to the location of the last item put in the stack */
    uint32_t *stackBase; /* Points to the base of the stack */
    uint32_t stackSize;      /* Size of the task's stack */
    TaskState state;         /* Current state of the task */
    //TODO uint32_t priority;       /* Priority of the task */
    void (*taskFunc)(void); /* Pointer to the task function */
} TaskControlBlock;

extern TaskControlBlock tasks[MAX_TASKS];
extern int currentTask;

void initTasks(void);
void createTask(void (*taskFunc)(void), int taskId);
void scheduler(Graphics_Context *context);

#endif /* SCHED_H_ */
