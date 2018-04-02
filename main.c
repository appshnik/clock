#include "clib.h"
#include "lcd_drv.h"
#include "sb_drv.h"

#define SET_GREEN	P1OUT |= (1<<6)
#define SET_RED		P1OUT |= (1<<0)
#define RESET_GREEN	P1OUT &= ~(1<<6)

extern struct hutemp ht_data;
extern int c_mode;
extern char top_str[17];
extern char bot_str[17];

int main(void)
{
	unsigned int humid = 0;
	/* initialization */
	init_device();
	lcd_init();
	wr_scr("kitchen clock", 0x01);
	P1DIR |= (1<<6) | (1<<0);
	/* main program loop */
	while (1) {
		__delay_ms(50UL);
		sb_read_data();
//		ht_data.hum_h = 0x02;
//		ht_data.hum_l = 0xB7;
		humid = ((unsigned int)ht_data.hum_h << 8) | ht_data.hum_l;
		__delay_ms(100UL);
		sprintf(bot_str, "%d.%d", humid/10, humid%10);
		wr_str(bot_str, 0x40);
		while (1);

//		wr_str("time", 0x46);
//		__delay_ms(1500UL);
//		wr_str("temperature", 0x42);
//		if (c_mode == MAIN_MD) {
		/* read data from RTC */
		/* read data from temperature sensor */
		/* write data to LCD */
			//TODO
//		}
//		else if (c_mode == SEL_MD) {
		/* write data to LCD */
			//TODO
//		} 
//		else if (c_mode == T_SET_MD) {
		/* write data to LCD */
			//TODO
//		}
//		else if (c_mode == C_SET_MD) {
		/* write data to LCD */
			//TODO
//		}
	}
	return 0;
}
