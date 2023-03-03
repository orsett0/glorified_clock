#pragma config FOSC=INTRC_NOCLKOUT
#pragma config WDTE=OFF
#pragma config PWRTE=ON
#pragma config MCLRE=OFF
#pragma config CP=OFF
#pragma config CPD=OFF
#pragma config BOREN=OFF
#pragma config IESO=OFF
#pragma config FCMEN=OFF
#pragma config LVP=ON
#pragma config DEBUG=OFF

#pragma config BOR4V=BOR21V
#pragma config WRT=OFF


#include <xc.h>
#include <proc/pic16f887.h>

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "display/display.h"
#include "ds1302/ds1302.h"


// This is redefined at compile time, but if i don't do this vscode screams.
#ifndef SEC_SINCE_EPOCH
#define SEC_SINCE_EPOCH 0
#endif


inline void flash(void) {
    RD4 = 1;
    __delay_ms(150);
    RD4 = 0;
}

int main(void) {
    TRISD4 = 0;
    RD4 = 1;

    char buff[17];

    RD4 = 0;
    while (1) {
        
    }
    
    return 0;
}
