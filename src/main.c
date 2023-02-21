#include <xc.h>
#include <proc/pic16f887.h>

#include "display/display.h"

#pragma config CONFIG1 = 0x3CF4
#pragma config CONFIG2 = 0x0600

int main() {
    TRISB0 = 0;
    RB0 = 1;

    displayinit(1);

    print("Hello, world!");
    __delay_ms(1000);

    char buff[] = "AAAAA";

    moveCursor(1, 30);
    print(buff);

    for (int i = 0; i < sizeof(buff) - 17; i++) {
        scrollDisplay();
        __delay_ms(200);
    }

    RB0 = 0;

    while (1) {
        RB0 = 1;
        __delay_ms(150);
        RB0 = 0;
        __delay_ms(1000);
    }
    
    return 0;
}
