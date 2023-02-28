// these are redefined at compile time,
// but if i don't do this vscode screams.
#ifndef SEC_SINCE_EPOCH
    #define SEC_SINCE_EPOCH 0
#endif


#pragma config CONFIG1 = 0x3CF4
#pragma config CONFIG2 = 0x0600


#include <xc.h>
#include <proc/pic16f887.h>

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "display/display.h"
#include "ds1302/ds1302.h"
#include "defines.h"


#define N_DATES 3
#define SPACE_PADDING 3


void printDiff(time_t);
void printCurtime(time_t curtime);
inline void flash(uint8_t);
void createCircularString(char *, char *, uint8_t, short);


// Use 'date -d"YYYY-MM-DD hh:mm:ss UTC+1" "+%s"' to populate the dates array.
time_t dates[N_DATES] = {
    1675438200,
    1672527600,
    1675206000,
};
char *descs[N_DATES] = {
    "16.30, 3 febbraio",
    "Capodanno",
    "Febbraio",
};
short index = -1;

short overflow;
short iter;
uint8_t int_exec_flag = 0;

/*
 * The interrupts are described at page 216, section 14.6 of the PIC16F887 datasheets
 * And at page 131, section 4.9 of the XC8 guide.
*/
// #ifdef __XC8 // vscode does not support xc8
// void __interrupt() handler(void) {
// #else
void handler() {
//#endif
    flash(0);
    
    //if (!INTF) return;

    if (++index == N_DATES) index = -1;
    else {
        overflow = strlen(descs[index]) - LCD_WIDTH;
        iter = 0;
    }

    int_exec_flag = 1;

    // Clear the interrupt
    INTF = 0;
}


int main() {
    char buff[LCD_WIDTH + 1];
    time_t curtime;
    uint8_t len;


    /* SETUP PORTS AND INTERRUPTS */

    // RD4 as an output
    TRISD4 = 0;
    RD4 = 0;

    // enable interrupt on falling edge
    ANSELH = 0;
    INTEDG = 0;
    INTE = 1;
    ei();


    /* INIT DISPLAY AND DS1302 */

    dsinit();
    displayinit(1);


    handler();
    //sendDateTime(SEC_SINCE_EPOCH);

    while (1) {
        __delay_ms(500); // Find the right value.

        di();

        curtime = SEC_SINCE_EPOCH; //recvDateTime();

        if (int_exec_flag) {
            int_exec_flag = 0;

            
            if (index >= 0) {
                clear();
                print(descs[index]);
                printDiff(difftime(curtime, dates[index]));
            }
        } else if (index < 0) 
            printCurtime(curtime);
        else if (overflow <= 0) 
            printDiff(difftime(curtime, dates[index]));
        else {
            if (iter == overflow + LCD_WIDTH) iter = -SPACE_PADDING;

            createCircularString(buff, descs[index], strlen(descs[index]), ++iter);
            buff[LCD_WIDTH] = '\0';

            moveCursor(0, 0);
            print(buff);
            
            printDiff(difftime(curtime, dates[index]));
        }

        ei();
    }
    
    return 0;
}

// There's probably a better way to do all of this
void createCircularString(char *buff, char *src, uint8_t src_len, short src_offset) {
    uint8_t to_copy;

    if (buff == NULL || src == NULL || src_len == 0) return;

    if (src_offset < 0) {
        strncpy(buff - src_offset, src, LCD_WIDTH + src_offset);
        while (src_offset + 1 < 0) buff[src_offset++ + SPACE_PADDING] = ' ';

        return;
    }

    to_copy = src_len - src_offset;

    strncpy(buff, src + src_offset, to_copy >= LCD_WIDTH ? LCD_WIDTH : to_copy);

    while (to_copy < src_len - src_offset + 2 && to_copy < LCD_WIDTH) buff[to_copy++] = ' ';

    if (++to_copy > LCD_WIDTH) return;

    strncpy(buff + to_copy, src, LCD_WIDTH - to_copy);
}

inline void flash(uint8_t state) {
    RD4 = state;
    __delay_ms(150);
    
    RD4 = !state;
    __delay_ms(150);

    RD4 = state;
}

void printDiff(time_t diff) {
    struct tm *datetime = localtime(&diff);
    char buff[41];

    // no need to clear if printing the whole line
    moveCursor(1, 0);

    // Sub 70 from the year because epoch is 1970 but time.h gives the years passed since 1900
    sprintf(buff, "%02dy %02dm %02dd     \0", datetime->tm_year - 70, datetime->tm_mon, datetime->tm_mday);
    print(buff);
}

void printCurtime(time_t curtime) {
    struct tm *datetime = localtime(&curtime);
    char buff[17];

    // no need to clear if printing the whole lines

    sprintf(buff, "   %02d/%02d/20%02d   \0", datetime->tm_mday, datetime->tm_mon, datetime->tm_year - 100);
    print(buff);

    moveCursor(1, 0);

    sprintf(buff, "    %02d:%02d:%02d    \0", datetime->tm_hour, datetime->tm_min, datetime->tm_sec);
    print(buff);
}
