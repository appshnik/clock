// #include <msp430.h>
#include "clib.h"
#include "lcd_drv.h"

#define TOGGLE_GREEN	P1OUT = P1OUT ^ (1<<6)

extern int c_mode;

int main(void)
{
	/* initialization */
	init_device();
	TOGGLE_GREEN;
	lcd_init();
	TOGGLE_GREEN;
	wr_scr("kitchen clock", 0x01);
	/* main program loop */
	while (1) {
		__delay_ms(1300UL);
		TOGGLE_GREEN;
		wr_str("time", 0x46);
		__delay_ms(1300UL);
		wr_str("temperature", 0x42);
//		if (c_mode == MAIN_MD) {
		/* read data from RTC */
		/* read data from temperature sensor */
		/* write data to LCD */
			;//TODO
//		}
//		else if (c_mode == SEL_MD) {
		/* write data to LCD */
			;//TODO
//		} 
//		else if (c_mode == T_SET_MD) {
		/* write data to LCD */
			;//TODO
//		}
//		else if (c_mode == C_SET_MD) {
		/* write data to LCD */
			;//TODO
//		}
	}
	return 0;
}
