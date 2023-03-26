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
#ifndef _XTAL_FREQ
#define _XTAL_FREQ      1
#endif

#ifndef SEC_SINCE_EPOCH
#define SEC_SINCE_EPOCH 1
#endif


char buff[17];


inline void flash(void) {
    RD4 = 1;
    __delay_ms(150);
    RD4 = 0;
}

void printCurtime(time_t curtime) {
    struct tm *datetime = localtime(&curtime);

    moveCursor(0, 0);
    sprintf(buff, "   %02d/%02d/20%02d   \0", datetime->tm_mday, datetime->tm_mon + 1, datetime->tm_year - 100);
    print(buff);

    moveCursor(1, 0);
    sprintf(buff, "    %02d:%02d:%02d    \0", datetime->tm_hour, datetime->tm_min, datetime->tm_sec);
    print(buff);
}

int main(void) {
    time_t t;
    uint8_t datetime[7];

    dsinit();
    displayinit(1);

    sendDateTime(SEC_SINCE_EPOCH);

    while (1) {
        t = recvDateTime();
        printCurtime(t);
        home();
        __delay_ms(500);
    }
    
    return 0;
}
