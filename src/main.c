#include <xc.h>
#include <proc/pic16f887.h>

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "display/display.h"
#include "ds1302/ds1302.h"

#define DAYOFWEEK 5

// This is redefined at compile time, but if i don't do this vscode screams.
#ifndef SEC_SINCE_EPOCH
#define SEC_SINCE_EPOCH 0
#endif

#pragma config CONFIG1 = 0x3CF4
#pragma config CONFIG2 = 0x0600

inline void flash(uint8_t state) {
    RD4 = !state;
    __delay_ms(200);
    RD4 = state;
}

void echo(uint8_t datetime[7]) {
    char buff[34];

    // C is disgusting. (in a nice way)
    sprintf(buff, "%d - %02d/%02d/%02d\0", datetime[5], datetime[3], datetime[4], datetime[6]);
    sprintf(buff + 17, "%02d:%02d:%02d", datetime[2], datetime[1], datetime[0]);

    print(buff);
    moveCursor(1, 0);
    print(buff + 17);
}

int main() {
    TRISD4 = 0;
    RD4 = 1;

    char buff[16 + 1];
    uint8_t datetime[7];

    displayinit(1);

    sendDateTime(SEC_SINCE_EPOCH);

    sprintf(buff, "%ld\0", SEC_SINCE_EPOCH);
    print(buff);
    __delay_ms(2000);
    clear();

    flash(1);
    __secondsToDate(SEC_SINCE_EPOCH, datetime);
    echo(datetime);
    __delay_ms(2000);
    clear();

    flash(1);
    sprintf(buff, "%ld\0", __dateToSeconds(datetime));
    print(buff);
    __delay_ms(2000);
    clear();

    flash(1);
    sprintf(buff, "%ld\0", recvDateTime());
    print(buff);
    __delay_ms(2000);
    clear();

    RD4 = 0;
    while (1) {
        flash(0);
        __delay_ms(1000);
    }
    
    return 0;
}
