#include "msp.h"
#include "task.h"
#include "semaphore.h"
#include "grlib.h"
#include "LcdDriver/Crystalfontz128x128_ST7735.h"

#define LED_DELAY 100000

Semaphore sem;

// Example task 1
void task1(void) {
    while (1) {
        semaphoreWait(&sem);
        P1->OUT ^= BIT0; // Toggle LED
        volatile int i;
        for (i = 0; i < LED_DELAY; i++); // Delay
        semaphoreSignal(&sem);
    }
}

// Example task 2
void task2(void) {
    while (1) {
        semaphoreWait(&sem);
        P2->OUT ^= BIT1; // Toggle another LED
        volatile int i;
        for (i = 0; i < LED_DELAY; i++); // Delay
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

    // Configure Ports

    initTasks();
    semaphoreInit(&sem, 1);

    createTask(task1, 0);
    createTask(task2, 1);

    while (1) {
        scheduler();
    }
}
