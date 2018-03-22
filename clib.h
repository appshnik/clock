#ifndef CLIB
#define CLIB

#include <stdint.h>
#include <msp430.h>

/* constants definition */
#define CPU_F		(1600000UL)		// oscilator frequency
#define STR_LEN		(16)			// LCD string length
#define MAIN_MD		(1)			// MAIN clock mode
#define SEL_MD		(2)			// select clock mode
#define C_SET_MD	(4)			// clock SET mode
#define T_SET_MD	(3)			// timer SET mode

/* IO definition */
#define SET_B		(1<<0)			// "set" button
#define UP_B		(1<<1)			// "set" button
#define DOWN_B		(1<<2)			// "set" button
#define NEXT_B		(1<<3)			// "set" button

/* event definition */
#define SET		(~P2IN & SET_B)		// "set" button is pressed
#define UP		(~P2IN & UP_B)		// "up" button is pressed
#define DOWN		(~P2IN & DOWN_B)	// "down" button is pressed
#define NEXT		(~P2IN & NEXT_B)	// "next" button is pressed

/* function macroses */
#define __delay_ms(ms)	__delay_cycles((CPU_F*ms)/1000UL)
#define __delay_us(us)	__delay_cycles((CPU_F*us)/1000000UL)
#define __delay_ns(ns)	__delay_cycles((CPU_F*ns)/1000000000UL)

/* function prototipes */
void init_device(void);

/* Types */
struct date {
	int yy;			// year
	uint8_t mth;		// month
	uint8_t dd;		// day
};
struct time {
	uint8_t hh;		// hours
	uint8_t mm;		// minutes
	uint8_t ss; 		// seconds
};
struct dt {
	struct date date;	// date
	struct time time;	// time
};

#endif // CLIB
