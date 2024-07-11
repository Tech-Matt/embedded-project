#include "utils.h"
#include <ti/grlib/grlib.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"

void logToLCD(Graphics_Context *context, const char* message) {
    static int yPos = 0;

    if (yPos > 120) {
        yPos = 0;
        Graphics_clearDisplay(&context);
    }

    Graphics_drawString(&context, (int8_t *)message, AUTO_STRING_LENGTH, 0, yPos, OPAQUE_TEXT);
    yPos += 10;
}
