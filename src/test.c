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
#include <string.h>

#include "display/display.h"
#include "defines.h"

// these are redefined at compile time,
// but if i don't do this vscode screams.
#ifndef SEC_SINCE_EPOCH
    #define SEC_SINCE_EPOCH 0
#endif


void printCurtime(time_t curtime);
inline void flash();
void adjustForLocaltime(time_t *);

char buff[LCD_WIDTH + 1];
uint8_t dst = 0;


int main() {
    time_t curtime;
    uint8_t len;

    TRISD4 = 0;
    RD4 = 0;

    displayinit(1);

    //sendDateTime(SEC_SINCE_EPOCH);

    while (1) {
        curtime = SEC_SINCE_EPOCH; //recvDateTime();
        adjustForLocaltime(&curtime);
    }
    
    return 1; // I mean we should never get here, so theres's clearly a problem.
}

inline void flash() {
    RD4 = 1;
    __delay_ms(100);
    RD4 = 0;
}

void printCurtime(time_t curtime) {
    struct tm *datetime = localtime(&curtime);

    moveCursor(0, 0);
    sprintf(buff, "   %02d/%02d/20%02d   \0", datetime->tm_mday, datetime->tm_mon, datetime->tm_year - 100);
    print(buff);

    moveCursor(1, 0);
    sprintf(buff, "    %02d:%02d:%02d    \0", datetime->tm_hour, datetime->tm_min, datetime->tm_sec);
    print(buff);
}

void adjustForLocaltime(time_t *t) {
    struct tm *lt;
    
    // Add seconds 3600 for CET
    (*t) += 3600;
    lt = localtime(t);

    // Last sunday of march, 2am -> set dst
    // Last sunday of october, 2am -> clear dst
    if (lt->tm_hour == 2 && lt->tm_wday == 0 && lt->tm_mon == 2 && lt->tm_mday - 7 > 23) dst = 1;
    else if (lt->tm_hour == 1 && lt->tm_wday == 0 && lt->tm_mon == 9 && lt->tm_mday - 7 > 23) dst = 0;

    // Daylight summer time, add 3600 seconds for CEST.
    if (dst) (*t) += 3600;
}
