# rtos
## Introduction
The goal of this project is to realize a small real time operating system for the TI MSP432 Board. The OS is gooing to have different functionalities, but the main objective is to be able to schedule different kinds of tasks, and to handle possible deadline violations, in order to keep the system in a safe state.
This is particularly useful in real life scenarios where there is a need for a system which, shouldn't fail at all, or at least fail in the most possible safe way, in order to minimize damage to people and / or systems.

You can learn more about the project in the bottom sections or here:
- Slides:
- Video:

## Supported Functionalities
- Preemptive Scheduler supporting different kinds of Tasks: Anytime, Soft, Hard.
- SysTick Interrupt
- Atomic Semaphores
- Simple Graphic Interface to log performed actions

## Dependencies
The project was created to 
You need to have a Library folder somewhere in your pc to import the following:
- grlib (see grlib.txt to see the path to add to the include files)
- Display driver (LcdDriver/Crystalfontz128x128_ST7735.h)

Add here instructions to set up driver lib and grlib 

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
## Semaphores
Every OS needs its semaphores implementation. This is no different for a real time os. In this case I defined three main functions in ```semaphore.h``` and ```semaphore.c```:
- ``` semaphoreInit() ```
- ``` semaphoreWait() ```
- ``` semaphoreSignal() ```

As straightforward as it sounds, ```semaphoreInit()``` initializes an integer semaphore, of a variable size. ```semaphoreWait()``` tries to lock a resource, if it is available, and ```semaphoreSignal()``` releases the lock on a resource.
The interesting thing is that each the wait() and signal() have been designed to be atomic. They use two specifics ARM instructions (```ldrex``` and ```strex```) to do that.

#### Implementation

## Synchronization

## Strenghts / Weaknesses

## API

