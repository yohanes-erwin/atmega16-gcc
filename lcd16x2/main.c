#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include "lcd16x2.h"

int main(void)
{
	// LCD initialization
	lcd16x2_init(LCD16X2_DISPLAY_ON_CURSOR_OFF_BLINK_OFF);
	
    while(1)
    {
        lcd16x2_puts("Hello, World!");
		_delay_ms(500);
		lcd16x2_clrscr();
		_delay_ms(500);
    }
}