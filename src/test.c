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
#include "display/display.h"
#include "ds1302/ds1302.h"


// This is redefined at compile time, but if i don't do this vscode screams.
#ifndef _XTAL_FREQ
#define _XTAL_FREQ      1
#endif

#define LCD_LED_TMR     10
#define MAX_COUNTER     ((uint8_t) (((unsigned long) _XTAL_FREQ) / (((unsigned long) 4) * ((unsigned long) 256) * ((unsigned long) 256))) * LCD_LED_TMR)


volatile uint8_t counter = 0;


inline void flash(void) {
    RD4 = 1;
    __delay_ms(150);
    RD4 = 0;
}

#ifdef __XC8
void __interrupt() handler(void)
#else
void handler(void)
#endif
{
    if (RBIF) {
        RBIF = 0;

        // Wait for bouncing and check if the button is pressed
        // 20ms is probably more than it's needed
        __delay_ms(20);
        if (RB1) return;

        RC3 = 1;

        // Enable timer
        T0IF = 0;
        TMR0 = 0x00;
        T0CS = 0;
        T0IE = 1;
    } else if (T0IF) {
        T0IF = 0;
        
        if (++counter < MAX_COUNTER) return;
        counter = 0;

        RC3 = 0;
        // Disable timer and interrupt
        T0CS = 1;
        T0IE = 0;
    }

    
}

int main(void) {
    char buff[9];

    displayinit(1);

    TRISD4 = 0;
    TRISC3 = 0;
    RD4 = 0;
    RC3 = 0;

    di();

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

    // Crear RBIF and globally enable interrupts
    RBIF = 0;
    ei();

    


    while (1) {
        sprintf(buff, "%d\0", MAX_COUNTER);
        print(buff);
        __delay_ms(1000);
    };
    
    return 0;
}
