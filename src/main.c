#include <clib.h>
#include <lcd_drv.h>
#include <sb_drv.h>

#define SET_GREEN	P1OUT |= (1<<6)
#define SET_RED		P1OUT |= (1<<0)
#define RESET_GREEN	P1OUT &= ~(1<<6)

extern struct hutemp ht_data;
extern int c_mode;
extern char top_str[STR_LEN];
extern char bot_str[STR_LEN];

int main(void)
{
	signed char ht_res;
	/* initialization */
	init_device();
	lcd_init();
	wr_scr("kitchen clock", 0x01);
	/* main program loop */
	while (1) {
		__delay_ms(2500UL);
		ht_res = sb_read_data();
		if (ht_res == -1)
			wr_str("Sensor error", 0x42);
		else {
			sprintf(bot_str, "%d.%d_%d.%d_%d", ht_data.hum/10, ht_data.hum%10, \
							ht_data.temp/10, ht_data.temp%10, \
							ht_res);
			wr_str(bot_str, 0x40);
		}
//		while (1);

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
