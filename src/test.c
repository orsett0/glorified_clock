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


/*
 * The interrupts are described at page 216, section 14.6 of the PIC16F887 datasheets
 * And at page 131, section 4.9 of the XC8 guide.
*/
#ifdef __XC8 // vscode does not support xc8
void __interrupt() handler(void) {
#else
void handler() {
#endif
    if (!INTF) return;

    RD4 = 1;
    __delay_ms(100);
    RD4 = 0;
    
    INTF = 0;
    RBIF = 0;
}

int main() {
    //__delay_us(100);

    ANSELH = 0;
    INTEDG = 0;
    INTE = 1;
    ei();

    TRISD4 = 0;
    RD4 = 0;
    RD4 = 1;
    __delay_ms(200);
    RD4 = 0;

    while (1) {}
    
    return 0;
}