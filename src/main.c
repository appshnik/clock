#include <common.h>
#include <board.h>
#include <lcd.h>
#include <sb.h>

#define _val(var)	((var)>=10)?(""):("0"), (var)

#define STR_LEN		(17)			/* LCD string length */

char top_str[STR_LEN];	/* top string of LCD */
char bot_str[STR_LEN];	/* bottom string of LCD */
uint8_t sb_rec_oper;	/* data reading is active */

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
		if (sb_rec_oper) {
			ht_res = sb_read_data();
			sb_rec_oper = 0;
		}
		/* data writing to RTC */
		if (dt_ch) {
			if (dt_ch == 1) {
				/*TODO: write date/time settings */;
			}
			else if (dt_ch == 2) {
				/* TODO: write timer settings */;
			}
			dt_ch = 0;
		}
		/* data output to LCD */
		switch (c_scr) {
		/* humidity/temperature screen */
		case HT_SCR:
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
		case DT_SCR:
			lcd_wr_str("current time", 0x02);
			sprintf(bot_str, \
				"%s%d:%s%d:%s%d  %s%d/%s%d", \
				_val(time.hh), \
				_val(time.mm), \
				_val(time.ss), \
				_val(date.dd), \
				_val(date.mth));
			lcd_wr_str(bot_str, 0x40);
			break;
		/* timer screen */
		case T_SCR:
			sprintf(top_str, \
				"timer state: %s",
				(timer.state)?("on"):("off"));
			lcd_wr_str(top_str, 0x00);
			sprintf(bot_str, \
				"%s%d:%s%d:%s%d left", \
				_val(timer.hh), \
				_val(timer.mm), \
				_val(timer.ss));
			lcd_wr_str(bot_str, 0x40);
			break;
		/* date/time setup */
		case DTS_SCR:
			sprintf(top_str, \
				"date: %s%d/%s%d/%d", \
				_val(date.dd), \
				_val(date.mth), \
				date.yy);
			lcd_wr_str(top_str, 0x00);
			sprintf(bot_str, \
				"time: %s%d:%s%d:%s%d", \
				_val(time.hh), \
				_val(time.mm), \
				_val(time.ss));
			lcd_wr_str(bot_str, 0x40);
			/* display cursor in corresponding position */
			lcd_wr_instr(_set_ddram(dt_curs[c_ind]));
			break;
		/* timer setup */
		case TS_SCR:
			sprintf(top_str, \
				"%s%d:%s%d:%s%d", \
				_val(timer.hh), \
				_val(timer.mm), \
				_val(timer.ss));
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



