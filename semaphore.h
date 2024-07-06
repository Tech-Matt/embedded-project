#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

typedef struct {
    volatile int value;
} Semaphore;

void semaphoreInit(Semaphore *sem, int value);
void semaphoreWait(Semaphore *sem);
void semaphoreSignal(Semaphore *sem);

#endif /* SEMAPHORE_H_ */
