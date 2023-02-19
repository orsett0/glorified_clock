#ifndef LCD_H
#define LCD_H

#include <stdint.h>
#include <stdio.h>
#include <xc.h>

#define LCD_CLEAR           0x01
#define LCD_HOME            0x02
#define LCD_ENTRYMODESET    0x04
#define LCD_DISPLAYCTRL     0x08
#define LCD_CDSHIFT         0x10
#define LCD_FUNCTIONSET     0x20
#define LCD_SETCGRAMADDR    0x40
#define LCD_SETDDRAMADDR    0x80

#define LCD_2LINE           0x08
#define LCD_ENTRYLEFT       0x02
#define LCD_ENTSHIFTINC     0x01

#define LCD_BLINKON         0x01
#define LCD_CURSORON        0x02
#define LCD_DISPLAYON       0x04

#define LCD_DISPLAYMOVE     0x08

uint8_t __dbus[8], __row_offsets[4];
uint8_t __rs, __rw, __en, __4bit;
uint8_t __display_ctrl = LCD_DISPLAYCTRL;
uint8_t __lines = 2; // Assume i'll always need two lines.

void displayinit(uint8_t, uint8_t, uint8_t, uint8_t, 
                uint8_t, uint8_t, uint8_t, uint8_t, 
                uint8_t, uint8_t, uint8_t);

void displayinit4bit(uint8_t, uint8_t, uint8_t, uint8_t,
                uint8_t, uint8_t, uint8_t);

void print(char *buff);

void clear();
void home();

void display(uint8_t);
void cursor(uint8_t);
void blink(uint8_t);

void scrollDisplay();
void customChar(uint8_t, char *);

void print(char *);

inline void __write(uint8_t);
inline void __command(uint8_t);
void __send(uint8_t, uint8_t);

void ____write4bits(uint8_t);
void ____write8bits(uint8_t);
inline void __pulseEnable();

#endif // LCD_H