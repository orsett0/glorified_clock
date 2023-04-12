// Copyright (C) 2023 Alessio Orsini <alessiorsini.ao@proton.me>
// SPDX-License-Identifier: GPL-3.0-or-later
// 
// A copy of the GNU General Public License is available in the 
// LICENSE file, in the project root directory.

#include "ds1302.h"

void dsinit(void) {
    DS_TRISCE = 0;
    DS_TRISSCLK = 0;
    
    DS_CE = 0;
    DS_SCLK = 0;
    
    // Clear write protect and clock halt bits.
    __send(0x00, 0x8E);
    __send(0x00, 0x80);
}

/*
 * Consider time to be seconds since epoch,
 * and sends the corresponding date to the DS1302
*/
void sendDateTime(time_t time) {
    uint8_t datetime[7];

    __secondsToDate(time, datetime);

    for (uint8_t i = 0; i < 6; i++)
        // bitwise and to be sure that CH and WP are not set accidentally.
        __send(0x7F & __toBCD(datetime[i]), 0x80 + (2 * i));

    // Can't &0x7F for the year, it needs the whole byte
    __send(__toBCD(datetime[6]), 0x8C);
}

/*
 * Receive a date and time from the DS1302
 * and return the corresponding seconds since epoch.
*/
time_t recvDateTime(void) {
    uint8_t datetime[7];

    for (uint8_t i = 0; i < 7; i++) 
        datetime[i] = __toBin(__recv(0x81 + (2 * i)));

    return __dateToSeconds(datetime);
;
}

/*
 * Converts a two-digits decimal value to the corresponding BCD
*/
uint8_t __toBCD(uint8_t value) {
    return (uint8_t) (((value / 10) << 4) | (value % 10));
}

/*
 * Converts BCD to a two digits decimal value
*/
uint8_t __toBin(uint8_t value) {
    return (value & 0x0F) + (((value >> 4) & 0x0F) * 10);
}

/*
 * Get seconds since epoch and put the date and time in *datetime
*/
void __secondsToDate(time_t time, uint8_t *datetime) {
    struct tm *curtime = localtime(&time);

    datetime[0] = (uint8_t) curtime->tm_sec;
    datetime[1] = (uint8_t) curtime->tm_min;
    datetime[2] = (uint8_t) curtime->tm_hour;
    datetime[3] = (uint8_t) curtime->tm_mday;
    datetime[4] = (uint8_t) curtime->tm_mon + 1;      // tm range 0-11
    datetime[5] = (uint8_t) curtime->tm_wday;
    datetime[6] = (uint8_t) curtime->tm_year - 100;   // tm gives the year since 1900, but I only care about the last two digits.
}

/*
 * Get date and time in *datetime and return a time_t rappresenting the sencond since epoch
*/
time_t __dateToSeconds(uint8_t *datetime) {
    struct tm curtime;

    curtime.tm_sec = datetime[0];
    curtime.tm_min = datetime[1];
    curtime.tm_hour = datetime[2];
    curtime.tm_mday = datetime[3];
    curtime.tm_mon = datetime[4] - 1;
    curtime.tm_wday = datetime[5];
    curtime.tm_year = datetime[6] + 100;

    return mktime(&curtime);
}

/*
 * Send 8 bit of data to the specified address
*/
void __send(uint8_t data, uint8_t addr) {
    __cmd(addr);
    __write(data);

    DS_CE = 0;
    DS_SCLK = 0;
    __delay_ms(4);
}

/*
 * Receive 8 bit of data from the spcified address
*/
uint8_t __recv(uint8_t addr) {
    uint8_t data = 0x00;

    __cmd(addr);

    DS_TRISIO = 1;
    for (uint8_t i = 0; i < 8; i++) {
        DS_SCLK = 1;
        __delay_us(1);
        DS_SCLK = 0;
        __delay_us(1);

        data |= (DS_IO << i);
    }

    DS_SCLK = 0;
    DS_CE = 0;

    return data;
}

/*
 * Send a command byte to the ds1302.
 * This function leaves CE and SCLK set.
*/
inline void __cmd(uint8_t addr) {
    DS_SCLK = 0;
    DS_CE = 1;
    __delay_us(4);
    
    /*
     * The addresses specified in the registers definition section of the datasheet (page 9)
     * IS the actual command byte specified at page 5.
     * The datasheety is not very clear in this, even tho the fact that the addresses specified
     * start at 0x80 should have made me suspicious.
     */
    __write(addr);
}

inline void __write(uint8_t data) {
    DS_TRISIO = 0;

    for (int i = 0; i < 8; i++) {
        DS_IO = (data >> i) & 0x01;
        __delay_us(1);

        DS_SCLK = 0;
        __delay_us(1);
        DS_SCLK = 1;
        __delay_us(1);
    }
}
