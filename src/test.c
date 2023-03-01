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
#include "ds1302/ds1302.h"
#include "defines.h"

// these are redefined at compile time,
// but if i don't do this vscode screams.
#ifndef SEC_SINCE_EPOCH
    #define SEC_SINCE_EPOCH 0
#endif
#define N_DATES 3
#define SPACE_PADDING 3


void printDiff(time_t);
void printCurtime(time_t curtime);
inline void flash();
void createCircularString(char *, uint8_t, short);
/*
 * These aren't useful. They would be only 
 * if they'd permit atomic comparisons between variables
*/
// inline uint8_t get(void *p);
// inline uint8_t set(void *p, uint8_t);
// inline uint8_t inc(void *p);
// inline uint8_t dec(void *p);


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
// Using signed char so that getter and setter works with the conversion to (uint8_t *)
int8_t index = -1;
char buff[LCD_WIDTH + 1];

int8_t overflow;
int8_t iter;

uint8_t int_exec_flag = 0;

/*
 * The interrupts are described at page 216, section 14.6 of the PIC16F887 datasheets
 * And at page 131, section 4.9 of the XC8 guide.
*/
#ifdef __XC8 // vscode does not support xc8
void __interrupt() handler(void) {
#else
void handler() {
#endif
    if (!INTF) return;

    flash();

    if (++index == N_DATES) index = -1;
    else {
        overflow = strlen(descs[index]) - LCD_WIDTH;
        iter = 0;
    }

    int_exec_flag = 1;

    // Clear the interrupt
    INTF = 0;
    //RBIF = 0;
}


int main() {
    time_t curtime;
    uint8_t len;

    /* SETUP PORTS AND INTERRUPTS */

    // Globally disable interrupts at boot
    di();

    // Set outputs
    TRISD4 = 0;
    RD4 = 0;

    // enable interrupt on falling edge
    ANSELH = 0;
    INTEDG = 0;
    INTE = 1;


    /* INIT DISPLAY AND DS1302 */

    dsinit();
    displayinit(1);

    //sendDateTime(SEC_SINCE_EPOCH);

    // Clearing th flag because the interrupts flag are set even if
    // they are globally disabled. (Note 1. page 216 of the datasheet)
    INTF = 0;
    // Globally enable interrupts
    ei();


    while (1) {
        curtime = SEC_SINCE_EPOCH; //recvDateTime();

        // The interrupt fired. Adjust to the new state and go on.
        if (int_exec_flag) {
            int_exec_flag = 0;

            if (index < 0) {
                printCurtime(curtime);
                continue;
            }

            // No need to check if overflow this time
            clear();
            print(descs[index]);
            printDiff(difftime(curtime, dates[index]));

            continue;
        }

        __delay_ms(500); // Find the right value.

        if (index < 0)
            printCurtime(curtime);
        else if (overflow <= 0)
            printDiff(difftime(curtime, dates[index]));
        else {
            if (iter == overflow + LCD_WIDTH) iter = -SPACE_PADDING;

            createCircularString(descs[index], strlen(descs[index]), ++iter);
            buff[LCD_WIDTH] = '\0';

            moveCursor(0, 0);
            print(buff);
            
            printDiff(difftime(curtime, dates[index]));
        }
    }
    
    return 1; // I mean we should never get here, so theres's clearly a problem.
}


/*
 * It is buggy. like a lot. don't ever try to change the number of spaces. (or anything else)
 * And don't ever even think to have a problem with this function. You'll regret all of you life choices.
 * If you need to change something, rewrite it from scratch. You'll figure something out.
*/
void createCircularString(char *src, uint8_t src_len, short src_offset) {
    uint8_t to_copy;

    if (src == NULL || src_len == 0) return;

    /*
     * Need to do this cause the last padding space doesn't get overwritten,
     * and i don't know what to change to make it work. Forgive me I'm sorry.
    */
    buff[0] = ' ';
    buff[1] = ' ';
    buff[2] = ' ';
    buff[3] = ' ';
    buff[4] = ' ';
    buff[5] = ' ';
    buff[6] = ' ';
    buff[7] = ' ';
    buff[8] = ' ';
    buff[9] = ' ';
    buff[10] = ' ';
    buff[11] = ' ';
    buff[12] = ' ';
    buff[13] = ' ';
    buff[14] = ' ';
    buff[15] = ' ';
    buff[16] = ' ';

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

inline void flash() {
    RD4 = 1;
    __delay_ms(100);
    RD4 = 0;
}

void printDiff(time_t diff) {
    struct tm *datetime = localtime(&diff);

    moveCursor(1, 0);

    // Sub 70 from the year because epoch is 1970 but time.h gives the years passed since 1900
    sprintf(buff, "%02dy %02dm %02dd     \0", datetime->tm_year - 70, datetime->tm_mon, datetime->tm_mday);
    print(buff);
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

// inline uint8_t get(void *p) {
//     uint8_t value;

//     di();
//     value = *((uint8_t *) p);
//     ei();

//     return value;
// }
// inline uint8_t set(void *p, uint8_t value) {
//     di();
//     *((uint8_t *) p) = value;
//     ei();

//     return value;
// }
// inline uint8_t inc(void *p) {
//     uint8_t value;
//     di();
//     *((uint8_t *) p) = *((uint8_t *) p) + 1;
//     value = *((uint8_t *) p);
//     ei();

//     return value;
// }
// inline uint8_t dec(void *p) {
//     uint8_t value;
//     di();
//     *((uint8_t *) p) = *((uint8_t *) p) - 1;
//     value = *((uint8_t *) p);
//     ei();

//     return value;
// }
