#ifndef DS1302_H
#define DS1302_H

#include <xc.h>
#include <stdint.h>
#include <stdlib.h>

#include "defines.h"

void dsinit();

void sendDateTime(uint8_t *);
void recvDateTime(uint8_t *);

inline void __write(uint8_t);

inline void __cmd(uint8_t, uint8_t, uint8_t);

uint8_t __recv(uint8_t, uint8_t);
void __send(uint8_t, uint8_t, uint8_t);

inline uint8_t __toBCD(uint8_t);
inline uint8_t __toBin(uint8_t value);

#endif // DS1302_H