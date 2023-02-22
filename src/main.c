#include <xc.h>
#include <proc/pic16f887.h>

#include "display/display.h"

#pragma config CONFIG1 = 0x3CF4
#pragma config CONFIG2 = 0x0600

uint8_t printCool(char *buff) {
    uint8_t i;
    blink(0);

    for (i = 0; buff[i] != '\0'; i++) {
        printc(buff[i]);
        if (i > 15) {
            scrollDisplay();
            __delay_ms(100);
        }

        __delay_ms(80); 
    }

    blink(1);
    __delay_ms(750);

    return i > 16 ? i - 16 : 0;
}

int main() {
    TRISB0 = 0;
    RB0 = 1;

    displayinit(1);

    cursor(1);
    blink(1);
    __delay_ms(2000);

    printCool("Hello, World!");

    moveCursor(1, 0);

    uint8_t n = printCool("orsetto - https://github.com/orsett0");
    if (n > 0) {
        home();
        moveCursor(1, n + 16);
    }

    __delay_ms(2000);

    blink(0);
    __delay_ms(500);
    cursor(0);
    __delay_ms(500);
    display(0);

    RB0 = 0;
    while (1) {
        RB0 = 1;
        __delay_ms(150);
        RB0 = 0;
        __delay_ms(1000);
    }
    
    return 0;
}
