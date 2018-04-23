#include <clib.h>
#include <lcd.h>
#include <sb.h>

int main(void)
{

	/* initialization */
	init_device();
	lcd_init();
	lcd_wr_scr("kitchen clock", 0x01);
	/* main program loop */
	while (1) {
		__delay_ms(2500UL);
		if (sb_rec_oper)
			ht_res = sb_read_data();
		if (ht_res == -1)
			lcd_wr_str("Sensor error", 0x42);
		else if (ht_res)
			lcd_wr_str("Data error", 0x43);
		else {
			sprintf(top_str, " hum: %d.%d%%", \
				ht_data.hum/10, ht_data.hum%10);
			lcd_wr_str(top_str, 0x00);
			sprintf(bot_str, "temp: %d.%d%cC", \
				ht_data.temp/10, ht_data.temp%10, 0xDF);
			lcd_wr_str(bot_str, 0x40);
		}
	}
	return 0;
}
