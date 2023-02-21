#include "display.h"

// This assumes the registers are properly configured from the user,
// and that's a bad assumption.
void displayinit(uint8_t fourbits)
{
    uint8_t display_func = LCD_2LINE;

    LCD_TRISRS = 0;
    LCD_TRISRW = 0;
    LCD_TRISEN = 0;
    LCD_TRISD4 = 0;
    LCD_TRISD5 = 0;
    LCD_TRISD6 = 0;
    LCD_TRISD7 = 0;

    //__4bit = fourbits;
    __4bit = 1;
    display_func |= __4bit ? 0x00 : LCD_8BITMODE;

    __row_offsets[0] = 0x00;
    __row_offsets[1] = 0x40;
    __row_offsets[2] = 0x10;
    __row_offsets[3] = 0x50;

    // Need to wait for the display to reach the right voltage.
    __delay_ms(50);

    LCD_RS = 0;
    LCD_RW = 0;
    LCD_EN = 0;

    // Apparently you need send the first command 3 times.
    if (__4bit) {
        __command(0x03);
        __delay_ms(5);
        __command(0x03);
        __delay_ms(5);
        __command(0x03);
        __delay_ms(5);
    } else {
        __command(LCD_FUNCTIONSET | display_func);
        __delay_ms(5);
        __command(LCD_FUNCTIONSET | display_func);
        __delay_ms(5);
    }

    home();
    __delay_ms(5);

    __command(LCD_FUNCTIONSET | display_func);
    __delay_ms(50);

    //__display_ctrl |= LCD_CURSORON;

    clear();
    __delay_ms(50);
    
    display(1);
    __delay_ms(50);

    __command(LCD_ENTRYMODESET | LCD_ENTRYLEFT);
    __delay_ms(50);
}

void printc(char c) {
    __write(c);
}

void print(char *buff) {
    if (buff == NULL) return;

    for (int i = 0; buff[i] != '\0'; i++) 
        __write(buff[i]);
}

void clear() {
    __command(LCD_CLEAR);
    __delay_ms(2);
}

void home() {
    __command(LCD_HOME);
    __delay_ms(2);
}

void display(uint8_t value) {
    if (value) __display_ctrl |= LCD_DISPLAYON;
    else __display_ctrl &= ~LCD_DISPLAYON;

    __command(LCD_DISPLAYCTRL | __display_ctrl);
}

void cursor(uint8_t value) {
    if (value) __display_ctrl |= LCD_CURSORON;
    else __display_ctrl &= ~LCD_CURSORON;

    __command(LCD_DISPLAYCTRL | __display_ctrl);
}

void blink(uint8_t value) {
    if (value) __display_ctrl |= LCD_BLINKON;
    else __display_ctrl &= ~LCD_BLINKON;

    __command(LCD_DISPLAYCTRL | __display_ctrl);
}

void scrollDisplay() {
    __command(LCD_LCDSHIFT | LCD_DISPLAYMOVE);
}

// This was on the arduino driver, but i might as well delete it.
void customChar(uint8_t location, char charmap[]) {
    location &= 0x7;
    __command(LCD_SETCGRAMADDR | (location << 3));
    for (int i = 0; i < 8; i++)
        __write(charmap[i]);
}

// The other examples account for 4 row.
// It might have something to do with the 40-char extension driver
// mentioned at page 12 of the datasheet, but it works like this too.
void moveCursor(uint8_t row, uint8_t col) {
    row = row > (__lines - 1) ? 0 : row;
    col = col > 40 ? 0 : col;

    __command(LCD_SETDDRAMADDR | (0x40 * row) + col);
}

inline void __write(uint8_t val) {
    __send(val, 1);
}

inline void __command(uint8_t val) {
    __send(val, 0);
}

void __send(uint8_t val, uint8_t mode) {
    LCD_RS = mode;
    LCD_RW = 0;

    if (__4bit) {
        __write4bits(val >> 4);
        __write4bits(val);
    }
    else __write8bits(val);
}

void __write4bits(uint8_t val) {
    for (int i = 4; i < 8; i++)
        __dbus[i] = (val >> (i - 4)) & 0x01;

    LCD_D4 = (val >> 0) & 0x01;
    LCD_D5 = (val >> 1) & 0x01;
    LCD_D6 = (val >> 2) & 0x01;
    LCD_D7 = (val >> 3) & 0x01;
    
    __pulseEnable();
}

void __write8bits(uint8_t val) {
    // for (int i = 0; i < 8; i++)
    //     __dbus[i] = (val >> i) & 0x01;

    // __pulseEnable();
}

inline void __pulseEnable() {
    LCD_EN = 0;
    __delay_us(1);
    LCD_EN = 1;
    __delay_us(1);
    LCD_EN = 0;
    __delay_us(100);
}