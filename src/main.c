#pragma config CONFIG1 = 0x3CF4
#pragma config CONFIG2 = 0x0600

#include "display/display.h"

int main() {
    __delay_us(1);

    TRISA0 = 0;
    TRISA1 = 0;
    TRISA2 = 0;
    TRISA3 = 0;

    RA0 = 0;
    RA1 = 0;
    RA2 = 0;
    RA3 = 0;

    TRISB0 = 0;
    RB0 = 1;

    displayinit(1);

    home();
    clear();
    display(1);

    print("Hello, world!");

    RB0 = 0;

    while (1) {
        RB0 = 1;
        __delay_ms(150);
        RB0 = 0;
        __delay_ms(1000);
    }
    
    return 0;
}
