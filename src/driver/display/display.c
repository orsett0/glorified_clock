#include "display.h"

// This assumes the registers are properly configured from the user,
// and that's a bad assumption.
void displayinit(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
                                 uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7,
                                 uint8_t rw, uint8_t rs, uint8_t en)
{
    uint8_t display_func = LCD_2LINE;

    __dbus[0] = d0;
    __dbus[1] = d1;
    __dbus[2] = d2;
    __dbus[3] = d3;
    __dbus[4] = d4;
    __dbus[5] = d5;
    __dbus[6] = d6;
    __dbus[7] = d7;

    __rs = rs;
    __rw = rw;
    __en = en;

    __4bit = (d4 & d5 & d6 & d7) == 0xFF;
    display_func = __4bit << 4;

    __row_offsets[0] = 0x00;
    __row_offsets[1] = 0x40;
    __row_offsets[2] = 0x10;
    __row_offsets[3] = 0x50;

    __delay_ms(50);

    __rs = 0;
    __rw = 0;
    __en = 0;

    // Apparently you need __send the first __command 3 times.
    if (__4bit)
    {
        ____write4bits(0x03);
        __delay_us(4500);
        ____write4bits(0x03);
        __delay_us(4500);
        ____write4bits(0x03);
        __delay_us(150);

        ____write4bits(0x02);
    }
    else
    {
        __command(display_func);
        __delay_us(4500);
        __command(display_func);
        __delay_us(150);
        __command(display_func);
    }

    __command(LCD_FUNCTIONSET | display_func);

    display(1);
    clear();

    __command(LCD_ENTRYMODESET | LCD_ENTRYLEFT);
}

void displayinit4bit(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
                                         uint8_t rw, uint8_t rs, uint8_t en)
{
    displayinit(d0, d1, d2, d3, 0xFF, 0xFF, 0xFF, 0xFF, rw, rs, en);
}

void print(char *buff) {
    if (buff == NULL) return;

    int i = 0;
    while (buff[i] != '\0') __write(buff[i]);
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
    __command(LCD_CDSHIFT | LCD_DISPLAYMOVE);
}

void customChar(uint8_t location, char charmap[]) {
    location &= 0x7;
    __command(LCD_SETCGRAMADDR | (location << 3));
    for (int i = 0; i < 8; i++)
        __write(charmap[i]);
}

inline void __write(uint8_t val) {
    __send(val, 1);
}

inline void __command(uint8_t val) {
    __send(val, 0);
}

void __send(uint8_t val, uint8_t mode) {
    __rs = mode;
    __rw = 0;

    if (__4bit) ____write4bits(val);
    else ____write8bits(val);
}

void ____write4bits(uint8_t val) {
    int i;

    for (i = 0; i < 4; i++)
        __dbus[i] = (val >> i) & 0x01;
    __pulseEnable();

    for (; i < 8; i++)
        __dbus[i] = (val >> i) & 0x01;
    __pulseEnable();
}

void ____write8bits(uint8_t val) {
    for (int i = 0; i < 8; i++)
        __dbus[i] = (val >> i) & 0x01;

    __pulseEnable();
}

inline void __pulseEnable() {
    __en = 0;
    __delay_us(1);
    __en = 1;
    __delay_us(1);
    __en = 0;
    __delay_us(100);
}