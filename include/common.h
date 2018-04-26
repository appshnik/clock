#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <msp430.h>

#define CPU_F		(20000000UL)		/* oscillator frequency */
#define TA_FAC		(CPU_F/1000000UL)	/* ticks per microsecond */
#define TA_CNT		(615)			/* number of TA0 cycles ~2 sec*/
#define STR_LEN		(17)			/* LCD string length */

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

/* global variables */
char top_str[STR_LEN];	/* top string of LCD */
char bot_str[STR_LEN];	/* bottom string of LCD */
uint8_t c_scr;		/* current screen number */
uint8_t c_ind;		/* current index */
uint8_t dt_ch;		/* flag that indicates date/time change */
struct date date;	/* current date */
struct time time;	/* current time */
struct timer timer;	/* current timer values */

/* cursor position array for date/time setup screen */
uint8_t dt_curs[] = {0x07, 0x0A, 0x0F, 0x47, 0x4A, 0x4D};
/* cursor position array for timer setup screen */
uint8_t t_curs[] = {0x01, 0x04, 0x07, 0x47};

/* array of pointers to  date/time parameters */
int *dt_ptr[] = {&date.dd, &date.mth, &date.yy, &time.hh, &time.mm, &time.ss};
/* array of pointers to  timer parameters */
uint8_t *t_ptr[] = {&timer.hh, &timer.mm, &timer.ss, &timer.state};

