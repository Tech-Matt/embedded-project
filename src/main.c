#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/devices/msp432p4xx/inc/msp.h>
#include "task.h"
#include "semaphore.h"
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include <ti/grlib/grlib.h>



#define LED_DELAY 100000

Semaphore sem;

// Declare global graphics context
Graphics_Context g_sContext;

void _gpio_init() {
    // Initialize LEDs as Output
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
}

void _graphicsInit()
{
    /* Initializes display */
    Crystalfontz128x128_Init();

    /* Set default screen orientation */
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);

    /* Initializes graphics context */
    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128, &g_sCrystalfontz128x128_funcs);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
    Graphics_clearDisplay(&g_sContext);

    Graphics_drawStringCentered(&g_sContext,
                                (int8_t *)"RTOS:",
                                AUTO_STRING_LENGTH,
                                64,
                                30,
                                OPAQUE_TEXT);

}

void _hw_init() {

    /* Halting WDT and disabling master interrupts */
    WDT_A_holdTimer();

    /* Initializes Clock System */


    // Initialize GPIOs
    _gpio_init();

    // Initialize graphics
    _graphicsInit();
}

// Example task 1
void task1(void) {
    // Lock
    semaphoreWait(&sem);
    GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0); // Toggle LED
    volatile int i;
    for (i = 0; i < LED_DELAY; i++); // Delay
    // Unlock
    semaphoreSignal(&sem);
}

// Example task 2
void task2(void) {
    // Lock
    semaphoreWait(&sem);
    GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN1); // Toggle another LED
    volatile int i;
    for (i = 0; i < LED_DELAY; i++); // Delay
    // Unlock
    semaphoreSignal(&sem);
}

int main(void) {

    _hw_init();

    initTasks();
    semaphoreInit(&sem, 1);

    createTask(task1, 0);
    createTask(task2, 1);

    while (1) {
        scheduler(&g_sContext);
    }
}
