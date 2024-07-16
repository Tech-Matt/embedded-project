#include "semaphore.h"
#include "task.h"

#include <stdio.h>
#include <arm_acle.h>

// Semaphore can be of any given size
//TODO consider if implementing atomic instructions also here
void semaphoreInit(Semaphore *sem, int value) {
    sem->value = value;
}

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


/* Same thing as above applies here */
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

