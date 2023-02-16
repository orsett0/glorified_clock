#include <xc.h>
#include <proc/pic16f887.h>

#pragma config CONFIG1 = 0x3CF4
#pragma config CONFIG2 = 0x0600

#define _XTAL_FREQ 8000000

int main()
{
  TRISB0 = 0; //RB0 as Output PIN
  while(1)
  {
    RB0 = 1; //LED ON
    __delay_ms(1000); //3 Second Delay
    RB0 = 0; //LED OFF
    __delay_ms(1000);
  }
  return 0;
}