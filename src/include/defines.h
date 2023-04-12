// Copyright (C) 2023 Alessio Orsini <alessiorsini.ao@proton.me>
// SPDX-License-Identifier: GPL-3.0-or-later
// 
// A copy of the GNU General Public License is available in the 
// LICENSE file, in the project root directory.

#ifndef DEFINES_H
#define DEFINES_H

#include <xc.h>
#include <proc/pic16f887.h>

#ifndef __XC8
typedef uint8_t __bit;
#endif

/*
 * ================================
 *   START LCD DRIVER DEFINITIONS
*/

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



/*
 * ================================
 * START DS1302 DRIVER DEFINITIONS
*/

#define DS_CE   RD3
#define DS_IO   RD2
#define DS_SCLK RD1

#define DS_TRISCE   TRISD3
#define DS_TRISIO   TRISD2
#define DS_TRISSCLK TRISD1


#endif