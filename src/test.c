// Copyright (C) 2023 Alessio Orsini <alessiorsini.ao@proton.me>
// SPDX-License-Identifier: GPL-3.0-or-later
// 
// A copy of the GNU General Public License is available in the 
// LICENSE file, in the project root directory.

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
struct tm *lt;
time_t dst_start, dst_end;


void adjustForLocaltime(time_t *t) {
    /*
     * The transition from CET to CEST happens at 1am UTC of the last sunday of march,
     * so i subtract the week day of the last day of march from that date 
     * in order to get the last sunday. (struct tm counts the week day considering sunday as 0)
     * The same concept is applied to the transition from CEST to CET
     */

    lt = localtime(t);

    lt->tm_mday = 31;
    lt->tm_min = 0;
    lt->tm_sec = 0;

    lt->tm_mon = 2;
    lt->tm_hour = 1;
    dst_start = mktime(lt);
    // xc8's (v2.36) implementation of mktime doesn't automatically adjust tm_wday
    // after a successful call, so I need to call localtime again.
    dst_start -= localtime(&dst_start)->tm_wday * 86400;

    lt->tm_mon = 9;
    lt->tm_hour = 1;
    dst_end = mktime(lt);
    dst_end -= localtime(&dst_end)->tm_wday * 86400;

    // Add seconds 3600 for CET
    (*t) += 3600;

    // Daylight summer time, add 3600 seconds for CEST.
    if ((*t) >= dst_start && (*t) < dst_end) (*t) += 3600;
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

    TRISA6 = 0;
    RA6 = 1;

    sendDateTime(SEC_SINCE_EPOCH);

    while (1) {
        t = recvDateTime();
        adjustForLocaltime(&t);

        home();
        printCurtime(t);

        __delay_ms(500);
    }
    
    return 0;
}
