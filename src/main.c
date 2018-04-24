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
		/* humidity/temperature screen */
		case 0:
			if (ht_res == -1)
				lcd_wr_scr("Sensor error", 0x42);
			else if (ht_res)
				lcd_wr_scr("Data error", 0x43);
			else {
				sprintf(top_str, \
					" hum: %d.%d%%", \
					ht_data.hum/10, ht_data.hum%10);
				lcd_wr_str(top_str, 0x00);
				sprintf(bot_str, \
					 "temp: %d.%d%cC", \
					ht_data.temp/10, ht_data.temp%10, 0xDF);
				lcd_wr_str(bot_str, 0x40);
			}
			break;
		/* date/time screen */
		case 2:
			lcd_wr_str("current time", 0x03);
			sprintf(bot_str, \
				"%2d:%2d:%2d %2d/%2d", \
				time.hh, time.mm, time.ss, \
				date.dd, date.mth);
			lcd_wr_str(bot_str, 0x40);
			break;
		/* timer screen */
		case 4:
			sprintf(top_str, \
				"timer state: %s",
				(timer.state)?("on"):("off"));
			lcd_wr_str(top_str, 0x00);
			sprintf(bot_str, \
				"%2d:%2d:%2d left", \
				timer.hh, timer.mm, timer.ss);
			lcd_wr_str(bot_str, 0x40);
			break;
		/* date/time setup */
		case 3:
			sprintf(top_str, \
				"date: %2d/%2d/%4d", \
				date.dd, date.mth, date.yy);
			lcd_wr_str(top_str, 0x00);
			sprintf(bot_str, \
				"time: %2d:%2d:%2d", \
				time.hh, time.mm, time.ss);
			lcd_wr_str(bot_str, 0x40);
			/* display cursor in corresponding position */
			lcd_wr_instr(_set_ddram(dt_curs[c_ind]));
			break;
		/* timer setup */
		case 5:
			sprintf(top_str, \
				"%2d:%2d:%2d", \
				timer.hh, timer.mm, timer.ss);
			lcd_wr_str(top_str, 0x00);
			sprintf(bot_str, \
				"timer %s",
				(timer.state)?("on"):("off"));
			lcd_wr_str(bot_str, 0x40);
			/* display cursor in corresponding position */
			lcd_wr_instr(_set_ddram(t_curs[c_ind]));
			break;
		}
	}
	return 0;
}
