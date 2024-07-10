#include "semaphore.h"
#include "task.h"

// Semaphore can be of any given size
void semaphoreInit(Semaphore *sem, int value) {
    sem->value = value;
}

/* Try to make the implementation safer. When the semaphore is available multiple tasks could try to enter the
 * critical zone at the same time. You should use atomic instructions. See if TI provides them
 */
void semaphoreWait(Semaphore *sem) {
    while (sem->value <= 0); // Polling
    sem->value--;
}

/* Same thing as above applies here */
void semaphoreSignal(Semaphore *sem) {
    sem->value++;
    scheduler();
}
