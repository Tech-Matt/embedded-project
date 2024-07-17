#include <sched.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/devices/msp432p4xx/inc/msp.h>
#include "semaphore.h"
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include <ti/grlib/grlib.h>


#define SYSTICK_PERIOD 1500000
#define LED_DELAY 100000

Semaphore sem;

// Declare global graphics context
Graphics_Context g_sContext;

/* Configuring SysTick to trigger at 1500000 (MCLK is 1.5MHz so this will make it toggle every 1s) */
void _systick_init() {
    // Disable the SysTick timer during setup
    SysTick_disableModule();
    SysTick_setPeriod(SYSTICK_PERIOD);
    Interrupt_enableSleepOnIsrExit();
    SysTick_enableInterrupt();
    SysTick_enableModule();
}

// Initialize LEDs
void _gpio_init() {
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

    int i;
    for (i = 0; i < LED_DELAY; i++); // Delay

    Graphics_clearDisplay(&g_sContext);

}

void _hw_init() {
    /* Initializes Clock System */
    //TODO Initialize clock

    // Initialize GPIOs
    _gpio_init();

    // Initialize graphics
    _graphicsInit();

    /* Initialize SysTick */
    _systick_init();

    /* Enabling MASTER interrupts */
    Interrupt_enableMaster();
}

// Example task 1
void task1(void) {
    semaphoreWait(&sem);
    GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0); // Toggle LED
    volatile int i;
    for (i = 0; i < LED_DELAY; i++); // Delay
    semaphoreSignal(&sem);
}

// Example task 2
void task2(void) {
    semaphoreWait(&sem);
    GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN1); // Toggle another LED
    volatile int i;
    for (i = 0; i < LED_DELAY; i++); // Delay
    semaphoreSignal(&sem);
}

int main(void) {
    /* Halting WDT and disabling master interrupts */
    WDT_A_holdTimer();

    _hw_init();

    initTasks();
    semaphoreInit(&sem, 1);

    createTask(task1, 0, 500, 1);
    createTask(task2, 1, 501, 1);

    while (1) {
        scheduler(&g_sContext);
    }
}

void SysTick_Handler(void)
{
    //TODO Implement call to the scheduler
    //TODO Implement elapsed time calculation
    //TODO Implement Task time violation handling
}
