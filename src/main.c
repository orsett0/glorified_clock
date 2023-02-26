#include <xc.h>
#include <proc/pic16f887.h>

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "display/display.h"
#include "ds1302/ds1302.h"

#define N_DATES 1

// This is redefined at compile time, but if i don't do this vscode screams.
#ifndef SEC_SINCE_EPOCH
#define SEC_SINCE_EPOCH 0
#endif

#pragma config CONFIG1 = 0x3CF4
#pragma config CONFIG2 = 0x0600

void printDiff(time_t);
void printCurtime(time_t curtime);
inline void flash(uint8_t);

time_t dates[N_DATES] = {0};
char *descs[N_DATES] = {0};
uint8_t index = 0;

short overflow;
uint8_t iter;

int main() {
    char buff[LCD_WIDTH + 1];
    uint8_t len;

    TRISD4 = 0;
    RD4 = 1;

    /*
     * Qui dovresti popolare le date.
     * Si tratta di parsare una stringa YYMMDD, popolare una struct tm, e usare localtime().
     * Non credo convenga popolare le strutture direttamente nel codice.
     * Potrei anche calcolare secondsSinceEpoch "a mano" per ogni data e inserire solo quello.
     * (ad esempio usa 'date -d"YYYY-MM-DD hh:mm:ss UTC+1" "+%s"')
     */

    RD4 = 0;
    while (1) {
        flash(0);
        __delay_ms(300); // Find the right value.

        time_t curtime = recvDateTime();

        if (index < 0) {
            printCurtime(curtime);
            continue;
        }

        if (overflow <= 0) {
            printDiff(difftime(curtime, dates[index]));
            continue;
        }

        len = strlen(descs[index] + iter);
        strncpy(buff, descs[index] + iter, LCD_WIDTH);

        // Pad the end with the start of descs[index]
        if (len < LCD_WIDTH - 3) {
            buff[len] = ' ';
            buff[len + 1] = ' ';
            buff[len + 2] = ' ';

            strncpy(buff + len + 3, descs[index], LCD_WIDTH - len - 3);
        }

        buff[LCD_WIDTH] = '\0';
        moveCursor(0, 0);
        print(buff);
        printDiff(difftime(curtime, dates[index]));

        if (iter == overflow + 16) iter = 0;
    }
    
    return 0;
}

void handler() { // TODO implement the interrupt
    if (++index == N_DATES) index = -1;
    overflow = strlen(descs[index]) - 16;
    iter = 0;

    clear();
    print(descs[index]);
}

inline void flash(uint8_t low) {
    RD4 = low;
    __delay_ms(200);
    RD4 = !low;
}

void printDiff(time_t diff) {
    struct tm *datetime = localtime(&diff);
    char buff[41];

    moveCursor(1, 0);
    sprintf(buff, "%02dy %02dm %02dd\0"); // You should make it pretty.
    print(buff);
}

void printCurtime(time_t curtime) {
    struct tm *datetime = localtime(&curtime);
    char buff[17];

    clear();

    sprintf(buff, "   %02d/%02d/20%02d   \0", datetime->tm_mday, datetime->tm_mon, datetime->tm_year);
    print(buff);

    moveCursor(1, 0);

    sprintf(buff, "    %02d:%02d:%02d    \0", datetime->tm_hour, datetime->tm_min, datetime->tm_sec);
    print(buff);
}
