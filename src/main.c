#include "msp.h"
#include "task.h"
#include "semaphore.h"
#include "Crystalfontz128x128_ST7735.h"
#include "HAL_MSP_EXP432P401R_Crystalfontz128x128_ST7735.h"
#include <ti/grlib/grlib.h>


#define LED_DELAY 100000

Semaphore sem;

// Declare global graphics context
Graphics_Context g_sContext;

void logToLCD(const char* message) {
    static int yPos = 0;

    if (yPos > 120) {
        yPos = 0;
        Graphics_clearDisplay(&g_sContext);
    }

    Graphics_drawString(&g_sContext, (int8_t *)message, AUTO_STRING_LENGTH, 0, yPos, OPAQUE_TEXT);
    yPos += 10;
}
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

    // Initialize LCD
    Crystalfontz128x128_Init();
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);

    // Initialize graphics context
    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128, &g_sCrystalfontz128x128_funcs);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_clearDisplay(&g_sContext);

    initTasks();
    semaphoreInit(&sem, 1);

    createTask(task1, 0);
    createTask(task2, 1);

    while (1) {
        scheduler();
    }
}
