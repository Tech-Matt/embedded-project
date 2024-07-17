#include <sched.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/devices/msp432p4xx/inc/msp.h>
#include "semaphore.h"
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include <ti/grlib/grlib.h>
#include "utils.h"


#define CLOCK 3000000
#define SYSTICK_PERIOD 12000000
#define LED_DELAY 100000
#define SCREEN_DELAY 200000
#define LONG_DELAY 400000

#define HARD_DEADLINE 12
#define SOFT_DEADLINE 8
#define ANYTIME_DEADLINE 8

Semaphore sem;

/* Keeps track of how long a task has been executing */
volatile uint32_t elapsed_seconds = 0;

// Keeps track of current task type in time violations handling
extern TaskType type;

// Declare global graphics context
Graphics_Context g_sContext;

/* Configuring SysTick to trigger at 1500000 (MCLK is 1.5MHz so this will make it toggle every 1s) */
void _systick_init() {
    /* Disable the SysTick timer during setup */
    SysTick_disableModule();
    /* The Core clock is 3MHZ so put any multiple to achieve the desired period */
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

    Graphics_drawString(&g_sContext, (int8_t *)"designed by:", AUTO_STRING_LENGTH, 30, 80, OPAQUE_TEXT);
    Graphics_drawString(&g_sContext, (int8_t *)"Mattia Rizzo", AUTO_STRING_LENGTH, 30, 95, OPAQUE_TEXT);

    int i;
    for (i = 0; i < SCREEN_DELAY; i++); // Delay

    Graphics_clearDisplay(&g_sContext);

}

void _hw_init() {
    /* Clock is left as default - 3 MHz */

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
    for (i = 0; i < LONG_DELAY; i++); // Delay
    semaphoreSignal(&sem);

}

// Example task 2
void task2(void) {
    semaphoreWait(&sem);
    GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN1); // Toggle another LED
    volatile int i;
    for (i = 0; i < LONG_DELAY; i++); // Delay
    semaphoreSignal(&sem);
}

int main(void) {
    /* Halting WDT and disabling master interrupts */
    WDT_A_holdTimer();

    _hw_init();

    initTasks();
    semaphoreInit(&sem, 1);

    createTask(task1, SOFT, 0, 500, 1);
    createTask(task2, HARD, 1, 501, 1);

    scheduler(&g_sContext);
}

/* This function gets called when a soft task doesn't end before the time constraint, in this case the scheduler gets called */
void soft_time_violation() {
    SysTick_disableInterrupt();
    logToLCD(&g_sContext, "Task Stopped");
    logToLCD(&g_sContext, "Releasing Semaphore");
    semaphoreSignal(&sem);
    logToLCD(&g_sContext, "Calling scheduler");
    int i;
    for(i = 0; i < SCREEN_DELAY; i++); //Delay
    scheduler(&g_sContext);
}

/* This function gets called when a hard task doesn't end before the time constraint. The msp432 is put in a safe state and
 * then rebooted.
 */
void hard_time_violation() {
    Interrupt_disableMaster();
    // Save stack, do useful stuff
    Graphics_clearDisplay(&g_sContext);

    Graphics_drawStringCentered(&g_sContext,
                                (int8_t *)"REBOOT",
                                AUTO_STRING_LENGTH,
                                64,
                                30,
                                OPAQUE_TEXT);

    int i;
    for (i = 0; i < 2 * LONG_DELAY; i++); // Delay

    SysCtl_rebootDevice();
}

void SysTick_Handler(void)
{
    elapsed_seconds = elapsed_seconds + (SYSTICK_PERIOD / CLOCK);

    /* ANYTIME TASK TIME VIOLATION */
    if (elapsed_seconds > ANYTIME_DEADLINE && type == ANYTIME) {
            logToLCD(&g_sContext, "Anytime Task took");
            logToLCD(&g_sContext, "too much time");
            logToLCD(&g_sContext, "Calling Scheduler");

            int i;
            for(i = 0; i < SCREEN_DELAY; i++); //Delay
            scheduler(&g_sContext);
        }

    /* SOFT TASK TIME VIOLATION */
    if (elapsed_seconds > SOFT_DEADLINE && type == SOFT) {
        logToLCD(&g_sContext, "Soft Task took too much time");
        int i;
        for(i = 0; i < SCREEN_DELAY; i++); //Delay
        soft_time_violation(); // Calling time violation routine
    }

    /* HARD TASK TIME VIOLATION */
    else if (elapsed_seconds > HARD && type == HARD) {
        logToLCD(&g_sContext, "Hard task has failed to comply");
        logToLCD(&g_sContext, "to time constraints.");
        logToLCD(&g_sContext, "SYSTEM HALTING NOW");
        int i;
        for(i = 0; i < SCREEN_DELAY; i++); //Delay
        hard_time_violation(); // Calling hard time violation routine
    }

}

