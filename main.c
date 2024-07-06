#include "msp.h"
#include "task.h"
#include "semaphore.h"

Semaphore sem;

void task1(void) {
    while (1) {
        semaphoreWait(&sem);
        P1->OUT ^= BIT0; // Toggle LED
        for (volatile int i = 0; i < 10000; i++); // Delay
        semaphoreSignal(&sem);
    }
}

void task2(void) {
    while (1) {
        semaphoreWait(&sem);
        P2->OUT ^= BIT1; // Toggle another LED
        for (volatile int i = 0; i < 10000; i++); // Delay
        semaphoreSignal(&sem);
    }
}

int main(void) {
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // Stop watchdog timer

    // Initialize LEDs
    P1->DIR |= BIT0;
    P1->OUT &= ~BIT0;
    P2->DIR |= BIT1;
    P2->OUT &= ~BIT1;

    initTasks();
    semaphoreInit(&sem, 1);

    createTask(task1, 0);
    createTask(task2, 1);

    while (1) {
        scheduler();
    }
}
