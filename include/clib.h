#ifndef CLIB_H
#define CLIB_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <msp430.h>

/* constants definition */
#define CPU_F		(20000000UL)		/* oscillator frequency */
#define TA_FAC		(CPU_F/1000000UL)	/* ticks per microsecond */
#define STR_LEN		(17)			/* LCD string length */
#define MAIN_MD		(1)			/* MAIN clock mode */
#define SEL_MD		(2)			/* select clock mode */
#define C_SET_MD	(4)			/* clock SET mode */
#define T_SET_MD	(3)			/* timer SET mode */

/* IO definition */
#define SET_B		(1<<0)			/* "set" button */
#define UP_B		(1<<1)			/* "up" button */
#define DOWN_B		(1<<2)			/* "down" button */
#define NEXT_B		(1<<3)			/* "next" button */

/* event definition */
#define SET		(~P1IN & SET_B)		/* "set" button is pressed */
#define UP		(~P1IN & UP_B)		/* "up" button is pressed */
#define DOWN		(~P1IN & DOWN_B)	/* "down" button is pressed */
#define NEXT		(~P1IN & NEXT_B)	/* "next" button is pressed */

/* function macros */
#define __delay_ms(ms)	(__delay_cycles((unsigned long)(CPU_F/1000UL * ms)))
#define __delay_us(us)	(__delay_cycles((unsigned long)(CPU_F/1000000UL * us)))

/* function definitions */
void init_device(void);

/* Types */
struct date {
	int yy;			/* year */
	uint8_t mth;		/* month */
	uint8_t dd;		/* day */
};
struct time {
	uint8_t hh;		/* hours */
	uint8_t mm;		/* minutes */
	uint8_t ss; 		/* seconds */
};
struct dt {
	struct date date;	/* date */
	struct time time;	/* time */
};

/* external variables */
extern struct hutemp ht_data;
extern uint8_t c_scr;
extern char top_str[STR_LEN];
extern char bot_str[STR_LEN];


#endif /* CLIB_H */
