#include "semaphore.h"
#include "task.h"

// Semaphore can be of any given size
void semaphoreInit(Semaphore *sem, int value) {
    sem->value = value;
}

// ATOMIC IMPLEMENTATION -- using LDREX, STREX
void semaphoreWait(Semaphore *sem) {
    int success;
    do {
        while(sem->value <= 0) {
            // busy wait
        }
    __asm volatile (
            "ldrex %0, [%1]\n"         // Load the semaphore value
            "subs    %0, %0, #1\n"     // Decrement the value
            "strex   %2, %0, [%1]\n"   // Attempt to store the new value
            : "=&r" (success)          // Output operands
            : "r" (&s->value), "0" (0) // Input operands
            : "cc"                     // Clobbered registers
        );
    } while (success != 0);            // Retry if the store was unsuccessful
}

/* Same thing as above applies here */
void semaphoreSignal(Semaphore *sem) {
    int success;
        int new_value;
        do {
            __asm volatile (
                "ldrex   %0, [%1]\n"                 // Load the semaphore value
                "adds    %0, %0, #1\n"               // Increment the value
                "strex   %2, %0, [%1]\n"             // Attempt to store the new value
                : "=&r" (new_value), "=&r" (success) // Output operands
                : "r" (&s->value)                    // Input operands
                : "cc"                               // Clobbered registers
            );
        } while (success != 0);            // Retry if the store was unsuccessful
}
