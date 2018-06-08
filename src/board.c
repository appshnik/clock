#include <common.h>
#include <board.h>
#include <lcd.h>
#include <sb.h>
#include <rtc.h>

#define TA_CNT		615			/* number of TA0 cycles ~2 sec*/

/* IO definition */
#define PREV_B		1<<0			/* "prev" button */
#define UP_B		1<<1			/* "up" button */
#define DOWN_B		1<<2			/* "down" button */
#define NEXT_B		1<<3			/* "next" button */
#define KEY_MSK		(PREV_B | \
			UP_B | \
			DOWN_B | \
			NEXT_B)			/* keyboard mask */
#define SB_MSK		0x08			/* single bus pin mask */

/* event definition */
#define PREV		(~P1IN & PREV_B)	/* "prev" button is pressed */
#define UP		(~P1IN & UP_B)		/* "up" button is pressed */
#define DOWN		(~P1IN & DOWN_B)	/* "down" button is pressed */
#define NEXT		(~P1IN & NEXT_B)	/* "next" button is pressed */

/* global variables */
uint8_t c_scr;		/* current screen number */
uint8_t c_ind;		/* current index */
uint8_t dt_ch;		/* indicates date/time change:  bit 0 - date changed
							bit 1 - time changed
							bit 2 - timer changed
							bit 3 - acknowledge */
struct date date;	/* current date */
struct time time;	/* current time */
struct timer timer;	/* current timer values */
struct timer remain;	/* timer remain values */
struct time alarm;	/* alarm time */

/* cursor position array for date/time setup screen */
uint8_t dt_curs[] = {0x07, 0x0A, 0x0F, 0x47, 0x4A, 0x4D};
/* cursor position array for timer setup screen */
uint8_t t_curs[] = {0x01, 0x04, 0x07, 0x47};

/* array of pointers to  date/time parameters */
int *dt_ptr[] = {&date.dd, &date.mth, &date.yy, &time.hh, &time.mm, &time.ss};
/* array of pointers to  timer parameters */
uint8_t *t_ptr[] = {&timer.hh, &timer.mm, &timer.ss, &timer.state};

/* disable WDT */
void wdt_init(void)
{
	WDTCTL = WDTPW | WDTHOLD;
}

/* ports 1 and 2 initialization */
void gpio_init(void)
{
	P1DIR = P1DIR & ~KEY_MSK;	/* set pins as inputs for buttons */
	P1SEL = P1SEL & ~KEY_MSK;	/* pin function set */
	P1SEL2 = P1SEL2 & ~KEY_MSK;	/* pin function set */
	P1SEL |= (BIT6 | BIT7);		/* I2C function */
	P1SEL2 |= (BIT6 | BIT7);	/* I2C function */
	P1IES = P1IES | KEY_MSK;
	P1IE = P1IE | KEY_MSK;

	P2IES = 0x00;
	P2IE = 0x00;
	P2SEL = 0x00;
	P2SEL2 = 0x00;

}
/* Timer A initialization */
void ta_init(void)
{
	TACTL = TASSEL_2;		/* SMCLK is used as clock source */
	TACTL |= MC_2;			/* continuous mode */
}

/* variables initialization */
void var_init(void)
{
	c_scr = 2;
	date.yy = 2018;
	date.mth = 0;
	date.dd = 4;
	time.hh = 2;
	time.mm = 22;
	time.ss = 5;
}

/* MCLK init as 20 MHz*/
void mclk_init(void)
{
	DCOCTL = 0xE0;			/* DCOx = 7	CPU_F ~ 20 MHz */
	BCSCTL1 = BCSCTL1 | 0x0F;	/* RSELx = 15 */
/*	DCOCTL = 0x60;			// DCOx = 3	CPU_F ~ 1 MHz
	BCSCTL1 = BCSCTL1 | 0x07;	// RSELx = 7 */

}

/* USCI init: I2C mode */
void usci_init(void)
{
	IE2 |= (UCB0TXIE | UCB0RXIE);
	while (UCB0CTL1 & UCTXSTP);
	UCB0CTL1 |= UCSWRST;
	/* UCB0 is master, I2C mode, synchronous mode */
	UCB0CTL0 |= UCMST | UCMODE_3 | UCSYNC;
	UCB0CTL1 |= UCSSEL_2;
	UCB0BR0 = 250;
	UCB0BR1 = 0;
	UCB0I2CSA = 0x68;
	UCB0CTL1 &= ~UCSWRST;

}

/* initialization routine */
void init_device(void)
{
	wdt_init();
	mclk_init();
	gpio_init();
	ta_init();
	__enable_interrupt();
}
