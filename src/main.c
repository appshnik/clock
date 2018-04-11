#include <clib.h>
#include <lcd_drv.h>
#include <sb_drv.h>


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
		else if (ht_res)
			wr_str("Data error", 0x43);
		else {
			sprintf(bot_str, "h:%d.%d__t:%d.%d", \
				ht_data.hum/10, ht_data.hum%10, \
				ht_data.temp/10, ht_data.temp%10);
			wr_str(bot_str, 0x40);
		}
	}
	return 0;
}
