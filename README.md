# rtos
## Introduction
The goal of this project is to realize a small real time operating system for the TI MSP432 Board. The OS is going to have different functionalities, but the main objective is to be able to schedule different kinds of tasks, and to handle possible deadline violations, in order to keep the system in a safe state.
This is particularly useful in real life scenarios where there is a need for a system which, shouldn't fail at all, or at least fail in the most possible safe way, in order to minimize damage to people and / or systems.

You can learn more about the project in the bottom sections or here:
- [Slides](https://drive.google.com/file/d/1Zn67WvD1QdIGSbo1DxQr8rlG5VtHItN5/view?usp=sharing)
- [Video](https://drive.google.com/file/d/1aUVh2lWh8voPJnwf7upPIvv_GKgKVdRo/view?usp=sharing)

## Supported Functionalities
- Preemptive Scheduler supporting different kinds of Tasks: Anytime, Soft, Hard.
- SysTick Interrupt
- Atomic Semaphores
- Simple Graphic Interface to log performed actions

## Dependencies
The Project was made using Code Composer Studio (Eclipse Version) and you need to include into your project include paths the following libraries:
- grlib (to use Boosterpack LCD)
- Display driver (LcdDriver/Crystalfontz128x128_ST7735.h)
- DriverLib (General HAL)

## Task Management
In this project 3 kind of tasks are took into consideration:
- **Anytime task**: this is the most common type of tasks. After its designed period of execution ends, the **scheduler** simply performs a _context switch_ and schedule the next task.
- **Soft Task**: this task should not exceed its designed period of execution, but if it does, even if there would be some problems, nothing catastrophics happens. The system still needs to call ```soft_time_violation()``` handler to perform some actions in order to put the system in a safe state.
- **Hard Task**: this is the most critical kind of task. The failure of this task to comply with the **timing constraints** may cause catastrophic consequences, like damage to people or systems, so in this implementation of the OS a ```hard_time_violation()``` routine is called, in order to put the system in a safe state, in the shortest time possible, and reboot it / shut it down.

#### Implementation
The Task type is defined by the following enum:

```c
typedef enum {
    ANYTIME,
    SOFT,
    HARD
} TaskType;
```
while each tasks come with some associated metadata:
```c
typedef struct {
    uint32_t pid;
    uint32_t counter; // example field to save task specific data
    uint32_t priority;
} TaskData;
```
## SysTick
To be able to preempt tasks that are currently executing we need an interrupt. In this case SysTick was specifically designed for this purpose, so let's use it.
This is the configuration:

```C
/* Configuring SysTick to trigger at 1500000 (MCLK is 1.5MHz so this will make it toggle every 1s) */
void _systick_init() {
    /* Disable the SysTick timer during setup */
    SysTick_disableModule();
    /* The Core clock is 3MHZ so put any multiple to achieve the desired period */
    SysTick_setPeriod(SYSTICK_PERIOD);
    Interrupt_enableSleepOnIsrExit();
    SysTick_enableInterrupt();
    SysTick_enableModule();
}
```

And this is the Interrupt Handler that it is called every 4 seconds:
```
void SysTick_Handler(void)
{
    elapsed_seconds = elapsed_seconds + (SYSTICK_PERIOD / CLOCK);

    /* ANYTIME TASK TIME VIOLATION */
    if (elapsed_seconds > ANYTIME_DEADLINE && type == ANYTIME) {
            logToLCD(&g_sContext, "Anytime Task took");
            logToLCD(&g_sContext, "too much time");
            logToLCD(&g_sContext, "Calling Scheduler");

            int i;
            for(i = 0; i < SCREEN_DELAY; i++); //Delay
            scheduler(&g_sContext);
        }

    /* SOFT TASK TIME VIOLATION */
    if (elapsed_seconds > SOFT_DEADLINE && type == SOFT) {
        logToLCD(&g_sContext, "Soft Task took too much time");
        int i;
        for(i = 0; i < SCREEN_DELAY; i++); //Delay
        soft_time_violation(); // Calling time violation routine
    }

    /* HARD TASK TIME VIOLATION */
    else if (elapsed_seconds > HARD && type == HARD) {
        logToLCD(&g_sContext, "Hard task has failed to comply");
        logToLCD(&g_sContext, "to time constraints.");
        logToLCD(&g_sContext, "SYSTEM HALTING NOW");
        int i;
        for(i = 0; i < SCREEN_DELAY; i++); //Delay
        hard_time_violation(); // Calling hard time violation routine
    }

}
```

In the ```Systick Interrupt Handler``` an important step of the RTOS is being executed. The variable _elapsedTime_ gets incremented and it gets then checked against deadline time conditions, to see if a task has been running for more time that it should. If so, exceptions routine will be called, such as:
- ``` soft_time_violation() ```
- ``` hard_time_violation() ```


## Scheduler
To have an effective scheduling policy we need to first keep track of some informations about the tasks we want to execute. We need a **Task Control Block**, let's see it:

```C
typedef struct {
    TaskState state;         /* Current state of the task */
    TaskType type;
    void (*taskFunc)(void);  /* Pointer to the task function */
    TaskData *stackPointer;  /* Pointer to the location of the task's data in the stack */
} TaskControlBlock;
```

Every task that is being created and then executed has an associated _TaskControlBlock_ that is then kept track of in an array:
```C
TaskControlBlock tasks[MAX_TASKS];
```

In scheduler.h the following functions are defined:
```C
void initTasks(void);
int createTask(void (*taskFunc)(void), TaskType type, uint32_t taskId, uint32_t pid, uint32_t priority);
void scheduler(Graphics_Context *context);
```

Let's see their implementation:

_initTask_
```C
void initTasks(void) {
    int i;
    for (i = 0; i < MAX_TASKS; i++) {
        tasks[i].state = TASK_SUSPENDED;
        tasks[i].taskFunc = NULL;
        tasks[i].stackPointer = NULL;
        tasks[i].type = ANYTIME;
    }
}
```

The _initTask_ function just initializes all the tasks in the array with default values.

## Semaphores
Every OS needs its semaphores implementation. This is no different for a real time os. In this case I defined three main functions in ```semaphore.h``` and ```semaphore.c```:
- ``` semaphoreInit() ```
- ``` semaphoreWait() ```
- ``` semaphoreSignal() ```

As straightforward as it sounds, ```semaphoreInit()``` initializes an integer semaphore, of a variable size. ```semaphoreWait()``` tries to lock a resource, if it is available, and ```semaphoreSignal()``` releases the lock on a resource.
The interesting thing is that each the wait() and signal() have been designed to be atomic. They use two specifics ARM instructions (```ldrex``` and ```strex```) to do that.

#### Implementation

Let's see the implementation for the ```wait``` and ```signal```.

```C
// ATOMIC IMPLEMENTATION -- using LDREX, STREX
void semaphoreWait(Semaphore *sem) {
    uint32_t tmp;
        do {
            while (sem->value <= 0) {
                // Busy wait
            }
            // Attempt to decrement the semaphore
            tmp = __ldrex((volatile uint32_t *)&sem->value);
            tmp--;
        } while (__strex(tmp, (volatile uint32_t *)&sem->value) != 0);

    // Memory barrier to ensure the operation is complete
    _dmb();
}


// ATOMIC IMPLEMENTATION -- using LDREX, STREX
void semaphoreSignal(Semaphore *sem) {
    uint32_t tmp;
        do {
            // Attempt to increment the semaphore
            tmp = __ldrex((volatile uint32_t *)&sem->value);
            tmp++;
        } while (__strex(tmp, (volatile uint32_t *)&sem->value) != 0);

        // Memory barrier to ensure the operation is complete
        _dmb();
}
```

```ldrex``` and ```strex``` are ARM-specific instructions used to implement atomic operations. They are crucial for ensuring that operations on shared resources in a concurrent environment are performed atomically, meaning that no other operations can interfere while they are being executed.
The ```ldrex``` reads the value from a memory location and marks it as being in the exclusive state. It essentially sets up a monitor on the address, indicating that the program intends to perform an atomic update on this address. ```strex``` attempts to write a new value to the same memory location that was previously loaded by ```ldrex```. The store will only succeed if no other writes have occurred to that location since the ```ldrex```. The result of the operation is returned: ```0``` if the store was successful and ```1``` if it was not.
