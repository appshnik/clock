#include <common.h>
#include <board.h>
#include <lcd.h>
#include <sb.h>
#include <rtc.h>

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

	rtc_write_ack();

	/* main program loop */
	while (1) {
		__delay_ms(500UL);


		/* data reading from HT sensor */
		if (sb_rec_oper) {
			ht_res = sb_read_data();
			sb_rec_oper = 0;
		}
		/* data reading from RTC */
		if (!dt_ch) {
			rtc_get_time();
			rtc_get_date();
		}
		if (timer.state) {
			rtc_get_timer_st();
		}
		/* data writing to RTC */
		if (c_scr == DT_SCR || c_scr == T_SCR) {
			/* write date/time settings */
			if (dt_ch & DS_CHANGED)
				rtc_write_date();
			else if (dt_ch & TS_CHANGED)
				rtc_write_time();
			else if (dt_ch & TMS_CHANGED)
				rtc_write_timer();
			else if (dt_ch & AL_ACK)
				rtc_write_ack();
			else if (dt_ch & STOP_ALARM) {
				rtc_write_stop_alarm();
				timer.state = 0;
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
			sprintf(top_str, \
				"current time");
			lcd_wr_str(top_str, 0x02);
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
				"%s%u:%s%u:%s%u left", \
				_val(remain.hh), \
				_val(remain.mm), \
				_val(remain.ss));
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
				"%s%u:%s%u:%s%u", \
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
		/* debugging screen */
		case 1:
			sprintf(top_str, \
				"alarm: %d:%d:%d", alarm.hh, alarm.mm, alarm.ss);
			lcd_wr_str(top_str, 0x00);
			sprintf(bot_str, \
				"cur_t: %s%d:%s%d:%s%d", \
				_val(time.hh), \
				_val(time.mm), \
				_val(time.ss));
			lcd_wr_str(bot_str, 0x40);
			break;
		}
	}
	return 0;
}



