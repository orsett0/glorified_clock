#ifndef DS1302_H
#define DS1302_H

#include <xc.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include "defines.h"

void dsinit();

void sendDateTime(time_t);
time_t recvDateTime();

inline void __write(uint8_t);

inline void __cmd(uint8_t, uint8_t, uint8_t);

uint8_t __recv(uint8_t, uint8_t);
void __send(uint8_t, uint8_t, uint8_t);

uint8_t __toBCD(uint8_t);
uint8_t __toBin(uint8_t);
void __secondsToDate(time_t, uint8_t *);
time_t __dateToSeconds(uint8_t *);

#endif // DS1302_H