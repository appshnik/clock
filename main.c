// #include <msp430.h>
#include "clib.h"
#include "lcd_drv.h"

extern int c_mode;

int main(void)
{
	if (c_mode == MAIN_MD) {
	/* read data from RTC */
	/* read data from temperature sensor */
	/* write data to LCD */
		;//TODO
	}
	else if (c_mode == SEL_MD) {
	/* write data to LCD */
		;//TODO
	} 
	else if (c_mode == T_SET_MD) {
	/* write data to LCD */
		;//TODO
	}
	else if (c_mode == C_SET_MD) {
	/* write data to LCD */
		;//TODO
	}
	
	return 0;
}
