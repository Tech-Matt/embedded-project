#include "semaphore.h"
#include "task.h"

void semaphoreInit(Semaphore *sem, int value) {
    sem->value = value;
}

void semaphoreWait(Semaphore *sem) {
    while (sem->value <= 0);
    sem->value--;
}

void semaphoreSignal(Semaphore *sem) {
    sem->value++;
    scheduler();
}
