#ifndef DEFINES_H
#define DEFINES_H

#include <xc.h>
#include <proc/pic16f887.h>

#define LCD_RS  RC0
#define LCD_RW  RC1
#define LCD_EN  RC2
#define LCD_D4  RC4
#define LCD_D5  RC5
#define LCD_D6  RC6
#define LCD_D7  RC7

#define LCD_TRISRS  TRISC0
#define LCD_TRISRW  TRISC1
#define LCD_TRISEN  TRISC2
#define LCD_TRISD4  TRISC4
#define LCD_TRISD5  TRISC5
#define LCD_TRISD6  TRISC6
#define LCD_TRISD7  TRISC7

#endif