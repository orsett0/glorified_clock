#include "ds1302.h"

void dsinit() {
    DS_TRISCE = 0;
    DS_TRISSCLK = 0;
    
    DS_CE = 0;
    DS_SCLK = 0;
    
    // Clear write protect and clock halt bits.
    __send(0x00, 0x8E, 0);
    __send(0x00, 0x80, 0);
}

void sendDateTime(uint8_t *datetime) {
    __send(0x7F & __toBCD(datetime[0]), 0x80, 0);   // Seconds, force clock halt bit to 0
    __send(__toBCD(datetime[1]), 0x82, 0);          // Minutes
    __send(0x7F & __toBCD(datetime[2]), 0x84, 0);   // Hour
    __send(__toBCD(datetime[3]), 0x86, 0);          // Date
    __send(__toBCD(datetime[4]), 0x88, 0);          // Month
    __send(__toBCD(datetime[5]), 0x8A, 0);          // Day of week
    __send(__toBCD(datetime[6]), 0x8C, 0);          // Year
}

void recvDateTime(uint8_t *datetime) {
    for (uint8_t i = 0; i < 7; i++) 
        datetime[i] = __recv(0x81 + (2 * i), 0);
}

inline uint8_t __toBCD(uint8_t value) {
    return ((value / 10) << 4) | (value % 10);
}

inline uint8_t __toBin(uint8_t value) {
    return (value & 0x0F) + ((value >> 4) * 10);
}

/*
 * Send 8 bit of data to the specified address
*/
void __send(uint8_t data, uint8_t addr, uint8_t ram) {
    __cmd(addr, 0, ram);
    __write(data);

    DS_CE = 0;
    DS_SCLK = 0;
    __delay_ms(4);
}

/*
 * Receive 8 bit of data from the spcified address
*/
uint8_t __recv(uint8_t addr, uint8_t ram) {
    uint8_t data = 0x00;

    __cmd(addr, 1, ram);

    DS_TRISIO = 1;
    for (uint8_t i = 0; i < 8; i++) {
        DS_SCLK = 1;
        __delay_us(1);
        DS_SCLK = 0;
        __delay_us(1);

        data |= DS_IO << i;
    }

    DS_SCLK = 0;
    DS_CE = 0;

    return data;
}

/*
 * Send a command byte to the ds1302.
 * This function leaves CE and SCLK set.
*/
inline void __cmd(uint8_t addr, uint8_t read, uint8_t ram) {
    DS_SCLK = 0;
    DS_CE = 1;
    __delay_us(4);
    
    __write(0x80 |
            ram ? 0x40 : 0x00 |
            (addr & 0x1F) << 1 |
            read ? 0x01 : 0x00);
}

inline void __write(uint8_t data) {
    for (int i = 0; i < 8; i++) {
        DS_IO = (data >> i) & 0x01;
        __delay_us(1);

        DS_SCLK = 0;
        __delay_us(1);
        DS_SCLK = 1;
        __delay_us(1);
    }
}
