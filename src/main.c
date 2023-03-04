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
#ifndef _XTAL_FREQ
#define _XTAL_FREQ      1
#endif

#define N_DATES         3
#define SPACE_PADDING   3

#define DEBOUNCE_TIME   20
#define LCD_LED_TMR     10
#define MAX_COUNTER     ((uint8_t) (((unsigned long) _XTAL_FREQ) \
                            / (((unsigned long) 4) * ((unsigned long) 256) * ((unsigned long) 256))) \
                            * LCD_LED_TMR)


void printDiff(time_t);
void printCurtime(time_t curtime);
inline void flash(void);
// using 16-bit integer for the string length to allow strings longer than 256
void createCircularString(char *, uint16_t, short);
void adjustForLocaltime(time_t *);
inline void turnOnLCD(void);


// Use 'date -d"YYYY-MM-DD hh:mm:ss" "+%s"' to populate the dates array.
// add 3600 for CET and 7200 for CEST
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
uint8_t dst = 0;

// 16 bit integer instead of 8 to allow strings bigger than (65535 - 17) chars.
int16_t overflow;
char buff[LCD_WIDTH + 1];
int8_t iter;

uint8_t int_exec_flag = 0;
volatile uint8_t counter = 0;

/*
 * The interrupts are described at page 216, section 14.6 of the PIC16F887 datasheets
 * And at page 131, section 4.9 of the XC8 guide.
*/
#ifdef __XC8 // vscode does not support xc8-cc
void __interrupt() handler(void)
#else
void handler()
#endif
{
    if (INTF) {
        INTF = 0;

        // Wait for debouncing and check if the button is pressed
        // 20ms is probably more than it's needed
        // This is necessary because a falling edge caused by a
        // bounce might trigger the interrupt
        __delay_ms(DEBOUNCE_TIME);
        if (RB0) return;

        if (++index == N_DATES) index = -1;
        int_exec_flag = 1;

        turnOnLCD();
        
        
    } else if (T0IF && !RBIF) {
        T0IF = 0;

        if (++counter < MAX_COUNTER) return;
        counter = 0;

        RA6 = 0;
        // Disable timer and interrupt
        T0CS = 1;
        T0IE = 0;
    } else if (RBIF) {
        RBIF = 0;
        
        // Wait for bouncing and check if the button is pressed
        // 20ms is probably more than it's needed
        __delay_ms(DEBOUNCE_TIME);
        if (RB1) return;

        turnOnLCD();
    }
}


int main() {
    time_t curtime;
    uint8_t len;

    /* SETUP PORTS AND INTERRUPTS */

    // Globally disable interrupts at boot
    //di();

    // PORTA
    ANSEL = 0;
    TRISA6 = 0;
    RA6 = 0;

    // Set outputs
    TRISD4 = 0;
    RD4 = 0;

    // enable interrupt on falling edge
    TRISB0 = 0;
    ANSELH = 0;
    INTEDG = 0;
    INTE = 1;

    // Enable interrupt on change on RB1
    ANSELH = 0;
    TRISB1 = 1;
    RBIE = 1;
    IOCB1 = 1;

    // Setup prescaler
    PSA = 0;
    PS2 = 1;
    PS1 = 1;
    PS0 = 1;

    // Read RB1 to latch the current value.
    // Don't know if it's really necessary.
    RB1;

    SSPEN = 0;

    /* INIT DISPLAY AND DS1302 */

    dsinit();
    displayinit(1);

    sendDateTime(SEC_SINCE_EPOCH);

    // Clearing flags because they are set even if interrupts are 
    // globally disabled. (Note 1. page 216 of the datasheet)
    INTF = 0;
    RBIF = 0;
    // Globally enable interrupts
    ei();


    // Flash to signal end of setup
    flash();

    sprintf(buff, "%d\0", MAX_COUNTER);
    print(buff);

    while (1) {
        curtime = recvDateTime();
        adjustForLocaltime(&curtime);

        // The interrupt fired. Adjust to the new state and go on.
        if (int_exec_flag) {
            int_exec_flag = 0;

            if (index < 0) {
                printCurtime(curtime);
                continue;
            }

            // Set overflow
            overflow = (int16_t) (strlen(descs[index]) - LCD_WIDTH);
            iter = 0;

            clear();
            print(descs[index]);
            printDiff((time_t) difftime(curtime, dates[index]));
            __delay_ms(500);

            continue;
        }

        //__delay_ms(500); // Find the right value.

        if (index < 0)
            printCurtime(curtime);
        else if (overflow <= 0)
            printDiff((time_t) difftime(curtime, dates[index]));
        else {
            if (iter == overflow + LCD_WIDTH) iter = -SPACE_PADDING;

            createCircularString(descs[index], (uint16_t) strlen(descs[index]), ++iter);
            buff[LCD_WIDTH] = '\0';

            moveCursor(0, 0);
            print(buff);
            
            printDiff((time_t) difftime(curtime, dates[index]));
            __delay_ms(500);
        }
    }
    
    return 1; // I mean we should never get here, so theres's clearly a problem.
}


/*
 * It is buggy. like a lot. don't ever try to change the number of spaces. (or anything else)
 * And don't ever even think to have a problem with this function. You'll regret all of you life choices.
 * If you need to change something, rewrite it from scratch. You'll figure something out.
*/
void createCircularString(char *src, uint16_t src_len, short src_offset) {
    uint16_t to_copy;

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
        strncpy(buff - src_offset, src, (size_t) (LCD_WIDTH + src_offset));
        while (src_offset + 1 < 0) buff[src_offset++ + SPACE_PADDING] = ' ';

        return;
    }

    to_copy = src_len - ((unsigned short) src_offset);

    strncpy(buff, src + src_offset, (size_t) (to_copy >= LCD_WIDTH ? LCD_WIDTH : to_copy));

    while (to_copy < src_len - ((unsigned short) src_offset) + 2 && to_copy < LCD_WIDTH) buff[to_copy++] = ' ';

    if (++to_copy > LCD_WIDTH) return;

    strncpy(buff + to_copy, src, (size_t) (LCD_WIDTH - to_copy));
}

inline void flash() {
    RD4 = 1;
    __delay_ms(500);
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
    sprintf(buff, "   %02d/%02d/20%02d   \0", datetime->tm_mday, datetime->tm_mon + 1, datetime->tm_year - 100);
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

inline void turnOnLCD() {
    RA6 = 1;
    counter = 0;

    // Enable timer
    T0IF = 0;
    TMR0 = 0x00;
    T0CS = 0;
    T0IE = 1;
}