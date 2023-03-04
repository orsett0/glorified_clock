#ifndef LCD_H
#define LCD_H

#include <stdint.h>
#include <stdio.h>

#include "defines.h"

#define LCD_CLEAR           0x01
#define LCD_HOME            0x02
#define LCD_ENTRYMODESET    0x04
#define LCD_DISPLAYCTRL     0x08
#define LCD_LCDSHIFT        0x10
#define LCD_FUNCTIONSET     0x20
#define LCD_SETCGRAMADDR    0x40
#define LCD_SETDDRAMADDR    0x80

#define LCD_2LINE           0x08
#define LCD_8BITMODE        0x10

#define LCD_ENTRYLEFT       0x02
#define LCD_ENTSHIFTINC     0x01

#define LCD_BLINKON         0x01
#define LCD_CURSORON        0x02
#define LCD_DISPLAYON       0x04

#define LCD_DISPLAYMOVE     0x08

#define LCD_WIDTH       16
#define LCD_MAX_CHAR        40

uint8_t __dbus[8], __row_offsets[4];
uint8_t __4bit;
uint8_t __display_ctrl = LCD_DISPLAYCTRL;
uint8_t __lines = 2; // Assume i'll always need two lines.

void displayinit(uint8_t);

/* Print content of buff to the display. */
void print(char *buff);
/* Print a single character to the display. */
void printc(char c);

/* Clear the display and set the cursor to (0, 0) */
void clear(void);
/* Move the cursor to (0, 0) */
void home(void);

/* Power on/off the display. */
void display(uint8_t);
/* Show/hide the cursor. */
void cursor(uint8_t);
/* Blink on/off. */
void blink(uint8_t);

/* Move the position of the cursor. */
void moveCursor(uint8_t, uint8_t);
/* Scroll the display to the left. */
void scrollDisplay();

/* Configure a new char (max 7). */
void customChar(uint8_t, char *);


/* Low-level functions */

inline void __data(uint8_t);
inline void __command(uint8_t);

void __instr(uint8_t, uint8_t);

void __write4bits(uint8_t);
void __write8bits(uint8_t);

inline void __pulseEnable();

#endif // LCD_H