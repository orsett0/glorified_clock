#include <xc.h>
#include <proc/pic16f887.h>

#pragma config CONFIG1 = 0x3CF4
#pragma config CONFIG2 = 0x0600

#include "display/display.h"

int main() {
    __delay_us(1);
    
    TRISA0 = 0;
    TRISA1 = 0;
    TRISA2 = 0;
    TRISA3 = 0;
    TRISA4 = 0;
    TRISA5 = 0;
    TRISA6 = 0;
    TRISA7 = 0;

    TRISC0 = 0;
    TRISC1 = 0;
    TRISC2 = 0;

    TRISB0 = 0;

    displayinit(RA0, RA1, RA2, RA3, RA4, RA5, RA6, RA7, RC1, RC0, RC2);

    RB0 = 1;

    home();
    clear();
    display(1);

    print("ciao");

    __delay_us(2000);
    RB0 = 0;

    while (1);
    
    return 0;
}
