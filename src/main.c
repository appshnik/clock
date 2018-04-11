#include <clib.h>
#include <lcd.h>
#include <sb.h>

int main(void)
{
	signed char ht_res;
	/* initialization */
	init_device();
	lcd_init();
	lcd_wr_scr("kitchen clock", 0x01);
	/* main program loop */
	while (1) {
		__delay_ms(2500UL);
		ht_res = sb_read_data();
		if (ht_res == -1)
			lcd_wr_str("Sensor error", 0x42);
		else if (ht_res)
			lcd_wr_str("Data error", 0x43);
		else {
			sprintf(bot_str, "h:%d.%d__t:%d.%d", \
				ht_data.hum/10, ht_data.hum%10, \
				ht_data.temp/10, ht_data.temp%10);
			lcd_wr_str(bot_str, 0x40);
		}
	}
	return 0;
}
