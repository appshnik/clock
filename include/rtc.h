#ifndef RTC_H
#define RTC_H

#include <stdint.h>

/* function definitions */
void rtc_get_time(void);
void rtc_get_date(void);
void rtc_write_time(void);
void rtc_write_date(void);
void rtc_write_timer(void);
void rtc_write_ack(void);
void rtc_write_stop_alarm(void);
void rtc_get_timer_st(void);
int bcd_nybble_to_int(uint8_t);
uint8_t bcd_to_int(uint8_t bcd);
uint8_t int_to_bcd(uint8_t val);

/* variables for global use*/
extern uint8_t rtc_rcv_buf[3];

#endif /* RTC_H */
