#include <xc.h>
#include <proc/pic16f887.h>

#include <stdint.h>

#pragma config CONFIG1 = 0x3CF4
#pragma config CONFIG2 = 0x0600

#define _XTAL_FREQ 8000000

int main()
{
    uint8_t state = 0;
    
    TRISD4 = 0;
    
    while (1) {
        RD4 = state;
        state = !state;
        __delay_ms(100);
    }


    return 0;
}