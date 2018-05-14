#include <common.h>
#include <i2c_bus.h>
#include <rtc.h>

#define RTC_ADDR	0x68		/* RTC module's I2C bus address */
#define RTC_BR_DIV	250		/* Divider for baud-rate calculation */

/* Global variables*/
uint8_t rtc_buf[5];

uint8_t rtc_time[] = {0x00};
uint8_t rtc_date[] = {0x04};
uint8_t rtc_timer[] = {0x07};
uint8_t rtc_al_contr[] = {0x0E};
uint8_t rtc_al_st[] = {0x0F};

uint8_t bcd_to_int(uint8_t bcd)
{
	int value;
	uint8_t msn = (bcd >> 4) & 0x0F;
	uint8_t lsn = bcd & 0x0F;

	value = 10 * msn + lsn;

	return value;
}

uint8_t int_to_bcd(uint8_t val)
{
	if (val <= 99)
		return ((val / 10) << 4) | (val % 10);
	else
		return 0;
}

/* checks if there is a slave with given address on the bus */
int rtc_check(uint8_t addr)
{
	i2c_transmitinit(addr, RTC_BR_DIV);
	while (i2c_busy());
	if (i2c_check_slave(addr))
		return 1;	/* slave exists */
	else
		return 0;	/* slave doesn't exist */
}
/* read given amount of bytes b_cnt to rtc_buf
from specified RTC register rtc_reg*/
int rtc_read(uint8_t b_cnt, uint8_t *rtc_reg)
{
	if (rtc_check(RTC_ADDR)) {
		/* write register pointer to RTC */
		i2c_transmitinit(RTC_ADDR, RTC_BR_DIV);
		i2c_transmit(1, rtc_reg);
		while (i2c_busy());

		i2c_receiveinit(RTC_ADDR, RTC_BR_DIV);
		while (i2c_busy());
		i2c_receive(b_cnt, rtc_buf);
		while (i2c_busy());

		return 1;
	} else {
		return 0;
	}
}
/* write given amount of bytes b_cnt from rtc_buf
to specified RTC register rtc_reg*/
int rtc_write(uint8_t b_cnt, uint8_t *rtc_reg)
{
	rtc_buf[0] = *rtc_reg;
	if (rtc_check(RTC_ADDR)) {
		/* write register pointer to RTC */
		i2c_transmitinit(RTC_ADDR, RTC_BR_DIV);
		i2c_transmit(b_cnt, rtc_buf);
		while (i2c_busy());

		return 1;
	} else {
		return 0;
	}
}

/* write current time to RTC */
void rtc_write_time(void)
{
	rtc_buf[1] = int_to_bcd(time.ss);
	rtc_buf[2] = int_to_bcd(time.mm);
	rtc_buf[3] = int_to_bcd(time.hh);

	rtc_write(4, rtc_time);
}

/* write current date to RTC */
void rtc_write_date(void)
{
	uint8_t cent = (date.yy >= 2000)?(1):(0);

	rtc_buf[1] = int_to_bcd(date.dd);
	rtc_buf[2] = int_to_bcd(date.mth) | (uint8_t)((cent)?(0x80):(0x00));
	rtc_buf[3] = int_to_bcd((date.yy % 100));

	rtc_write(4, rtc_date);
}

/* write timer settings to RTC */
void rtc_write_timer(void)
{
	uint8_t rem = 0;
	/* calculate alarm seconds */
	alarm.ss = (time.ss + timer.ss)%60;
	rem = (time.ss + timer.ss)/60;
	/* calculate alarm minutes */
	alarm.mm = (time.mm + timer.mm + rem)%60;
	rem = (time.mm + timer.mm + rem)/60;
	/* calculate alarm hours */
	alarm.hh = (time.hh + timer.hh + rem)%24;

	rtc_buf[1] = int_to_bcd(alarm.ss);
	rtc_buf[2] = int_to_bcd(alarm.mm);
	rtc_buf[3] = int_to_bcd(alarm.hh);
	rtc_buf[4] = 0x80;
	/* write alarm time */
	rtc_write(5, rtc_timer);
	if (timer.state) {
		rtc_buf[1] = 0x05;	/* enable alarm OUT */
		rtc_write(2, rtc_al_contr);
	}
}

/* write alarm acknowledgement to RTC */
void rtc_write_ack(void)
{
	rtc_buf[1] = 0x00;

	rtc_write(2, rtc_al_st);
}

/* write alarm switch off command to RTC */
void rtc_write_stop_alarm(void)
{
	rtc_buf[1] = 0x04;

	rtc_write(2, rtc_al_contr);
}

/* calculate timer remain */
void rtc_timer_remain()
{
	unsigned long int now_s = time.hh * 3600L + time.mm * 60 + time.ss;
	unsigned long int alm_s = alarm.hh * 3600L + alarm.mm * 60 + alarm.ss;
	unsigned long int rem_s = 86400L - now_s + alm_s;


	if (!remain.state) {
		remain.ss = rem_s % 60;
		remain.mm = (rem_s / 60) % 60;
		remain.hh = ((rem_s / 60) /60) % 24;
	} else {
		remain.ss = 0;
		remain.mm = 0;
		remain.hh = 0;
	}
}

/* get RTC timer state */
void rtc_get_timer_st(void)
{
	if (rtc_read(1, rtc_al_st)) {
		remain.state = rtc_buf[0] & 0x01;
	}
	rtc_timer_remain();
}

/* get current RTC time */
void rtc_get_time(void)
{
	if (rtc_read(3, rtc_time)) {
		time.hh = bcd_to_int(rtc_buf[2] & 0x3F);
		time.mm = bcd_to_int(rtc_buf[1]);
		time.ss = bcd_to_int(rtc_buf[0]);
	} else {
		time.hh = 99;
		time.mm = 99;
		time.ss = 99;
	}
}

/* get current RTC date */
void rtc_get_date(void)
{

	if (rtc_read(3, rtc_date)) {
		uint8_t bcd_mth = rtc_buf[1];
		date.mth = bcd_to_int(bcd_mth & 0x1F);
		date.dd = bcd_to_int(rtc_buf[0]);
		date.yy = (int)(bcd_to_int(rtc_buf[2])) + \
			  ((bcd_mth & 0x80)?(2000):(1900));
	} else {
		date.yy = 99;
		date.mth = 99;
		date.dd = 99;
	}
}
