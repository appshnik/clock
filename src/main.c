#include <clib.h>
#include <lcd.h>
#include <sb.h>

int main(void)
{

	/* initialization */
	init_device();
	lcd_init();
	lcd_wr_str("Kitchen clock", 0x01);
	lcd_wr_str("Initialization", 0x40);
	__delay_ms(500UL);
	lcd_wr_str("Initialization.", 0x40);
	__delay_ms(500UL);
	lcd_wr_str("Initialization..", 0x40);
	__delay_ms(500UL);
	/* main program loop */
	while (1) {
		__delay_ms(500UL);
		/* data reading from HT sensor */
		if (sb_rec_oper)
			ht_res = sb_read_data();
		/* data output to LCD */
		switch (c_scr) {
		case 0:
			if (ht_res == -1)
				lcd_wr_scr("Sensor error", 0x42);
			else if (ht_res)
				lcd_wr_scr("Data error", 0x43);
			else {
				sprintf(top_str, " hum: %d.%d%%", \
					ht_data.hum/10, ht_data.hum%10);
				lcd_wr_str(top_str, 0x00);
				sprintf(bot_str, "temp: %d.%d%cC", \
					ht_data.temp/10, ht_data.temp%10, 0xDF);
				lcd_wr_str(bot_str, 0x40);
			}
			break;
		case 2:
			lcd_wr_str("date screen", 0x00);
			break;
		case 4:
			lcd_wr_str("timer screen", 0x00);
			break;
		case 3:
			lcd_wr_str("date setup", 0x00);
			break;
		case 5:
			lcd_wr_str("timer setup", 0x00);
			break;
		}
		sprintf(bot_str, "c_scr:%d c_ind:%d", c_scr, c_ind);
		lcd_wr_str(bot_str, 0x40);
	}
	return 0;
}
