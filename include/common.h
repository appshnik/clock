#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <msp430.h>

#define CPU_F		(20000000UL)		/* oscillator frequency */
#define TA_FAC		(CPU_F/1000000UL)	/* ticks per microsecond */

/* function macros */
#define __delay_ms(ms)	(__delay_cycles((unsigned long)(CPU_F/1000UL * ms)))
#define __delay_us(us)	(__delay_cycles((unsigned long)(CPU_F/1000000UL * us)))

/* Types */
struct date {
	int yy;			/* year */
	int mth;		/* month */
	int dd;			/* day */
};
struct time {
	int hh;			/* hours */
	int mm;			/* minutes */
	int ss; 		/* seconds */
};
struct timer {
	uint8_t hh;		/* hours */
	uint8_t mm;		/* minutes */
	uint8_t ss; 		/* seconds */
	uint8_t state;		/* timer state: on/off */
};
struct hutemp {
	uint8_t hum_h;		/* humidity high byte */
	uint8_t hum_l;		/* humidity low byte */
	uint8_t temp_h;		/* temperature high byte */
	uint8_t temp_l;		/* temperature low byte */
	uint8_t ch_sum;		/* check sum */
	int hum;		/* current humidity */
	int temp;		/* current humidity */
};

/* external variables */
extern uint8_t dt_ch;		/* flag that indicates date/time change */
extern struct time time;
extern struct timer timer;
extern struct date date;
extern struct hutemp ht_data;
extern uint8_t sb_rec_oper;

#endif	/* COMMON_H */
