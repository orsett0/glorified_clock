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

void echo(uint8_t datetime[7]) {
    char buff[17];

    moveCursor(0, 0);
    sprintf(buff, "%d - %02d/%02d/%02d    \0", datetime[5], datetime[3], datetime[4], datetime[6]);
    print(buff);

    moveCursor(1, 0);
    sprintf(buff, "%02d:%02d:%02d        \0", datetime[2], datetime[1], datetime[0]);
    print(buff);
}
void echoBCD(uint8_t datetime[7]) {
    char buff[17];

    moveCursor(0, 0);
    sprintf(buff, "%x - %02x/%02x/%02x    \0", datetime[5], datetime[3], datetime[4], datetime[6]);
    print(buff);

    moveCursor(1, 0);
    sprintf(buff, "%02x:%02x:%02x        \0", datetime[2], datetime[1], datetime[0]);
    print(buff);
}

int main(void) {
    TRISD4 = 0;
    RD4 = 1;

    char buff[17];

    uint8_t *datetime;
    time_t rec;

    dsinit();
    displayinit(1);

    sendDateTime(SEC_SINCE_EPOCH);

    RD4 = 0;
    while (1) {
        //flash();
        //datetime = recvDateTime(&rec);
        //echo(datetime);

        sprintf(buff, "%ld\0", recvDateTime(&rec));
        home();
        print(buff);
    }
    
    return 0;
}
