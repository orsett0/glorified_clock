#include <xc.h>
#include <proc/pic16f887.h>

#include "display/display.h"

#pragma config CONFIG1 = 0x3CF4
#pragma config CONFIG2 = 0x0600

int main() {
    TRISB0 = 0;
    RB0 = 1;

    displayinit(1);

    cursor(1);
    blink(1);
    __delay_ms(2000);

    blink(0);
    char buff[] = "Hello world!";
    for (int i = 0; i < sizeof(buff) - 1; i++) {
        printc(buff[i]);
       __delay_ms(80); 
    }

    blink(1);
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
