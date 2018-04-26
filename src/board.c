#include <common.h>
#include <board.h>
#include <lcd.h>
#include <sb.h>

#define TA_CNT		(615)			/* number of TA0 cycles ~2 sec*/

/* IO definition */
#define PREV_B		(1<<0)			/* "prev" button */
#define UP_B		(1<<1)			/* "up" button */
#define DOWN_B		(1<<2)			/* "down" button */
#define NEXT_B		(1<<3)			/* "next" button */
#define KEY_MSK		(PREV_B | \
			UP_B | \
			DOWN_B | \
			NEXT_B)			/* keyboard mask */
#define SB_MSK		(0x08)			/* single bus pin mask */

/* event definition */
#define PREV		(~P1IN & PREV_B)	/* "prev" button is pressed */
#define UP		(~P1IN & UP_B)		/* "up" button is pressed */
#define DOWN		(~P1IN & DOWN_B)	/* "down" button is pressed */
#define NEXT		(~P1IN & NEXT_B)	/* "next" button is pressed */

/* global variables */
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

/* disable WDT */
void wdt_init(void)
{
	WDTCTL = WDTPW | WDTHOLD;
}

/* ports 1 and 2 initialization */
void gpio_init(void)
{
	P2IES = 0x00;
	P2IE = 0x00;

	P1DIR = P1DIR & ~KEY_MSK;	/* set pins as inputs for buttons */
	P1SEL = P1SEL & ~KEY_MSK;	/* disable pin multiplexing */
	P1SEL2 = P1SEL2 & ~KEY_MSK;	/* disable pin multiplexing */
	P1IES = P1IES | KEY_MSK;
	P1IE = P1IE | KEY_MSK;

	P2SEL &= ~SB_MSK;
	P2SEL2 &= ~SB_MSK;

}
/* Timer A initialization */
void ta_init(void)
{
	TACTL = TASSEL_2;		/* SMCLK is used as clock source */
	TACTL |= MC_2;			/* continuous mode */
	TACTL |= TAIE;			/* enable interrupt request */
}

/* variables initialization */
void var_init(void)
{
	c_scr = 0;
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
/* initialization routine */
void init_device(void)
{
	__enable_interrupt();
	wdt_init();
	mclk_init();
	gpio_init();
	ta_init();
	var_init();
}

/* write next less significant bit */
void wr_next_bit(signed char bit, uint8_t *byte, uint8_t bit_cnt)
{
	if (bit)
		*byte |= 1;
	else
		*byte &= ~1;
	/* shift byte for next writing */
	if (bit_cnt % 8 < 7)
		*byte = *byte << 1;
}

/* port 2 interrupt service routine */
__attribute__((interrupt(PORT2_VECTOR)))
/* cppcheck-suppress unusedFunction */
void p2_isr(void)
{
	/* temperature and humidity data reading */
	signed char bit;
	if (P2IFG & SB_MSK) {
		bit = sb_get_bit();
		/* ignore first short impulse from sensor */
		if (bit < 0 && bit_count == 0)
			goto p2_isr_end;
		if ((bit >= 0)) {
			switch(bit_count / 8) {
			case 0	:
				wr_next_bit(bit, &(ht_data.hum_h), bit_count);
				break;
			case 1	:
				wr_next_bit(bit, &(ht_data.hum_l), bit_count);
				break;
			case 2	:
				wr_next_bit(bit, &(ht_data.temp_h), bit_count);
				break;
			case 3	:
				wr_next_bit(bit, &(ht_data.temp_l), bit_count);
				break;
			case 4	:
				wr_next_bit(bit, &(ht_data.ch_sum), bit_count);
				break;
			}
		}
		bit_count++;
		if ((bit_count >= 40) || (bit < 0)) {
			bit_count = 0;
			P2IE &= ~SB_MSK;
		}
	p2_isr_end:
		P2IFG = P2IFG & ~SB_MSK;
	}
}

/* TimerA interrupt service routine */
__attribute__((interrupt(TIMER0_A1_VECTOR)))
/* cppcheck-suppress unusedFunction */
void ta0_isr(void)
{
	static int sb_wait_cnt;

	TACTL = TACTL & ~TAIFG;
	/* waiting for the time to read from HT sensor*/

	if (sb_wait_cnt < TA_CNT) {
		sb_wait_cnt += 1;
	}
	else {
		sb_wait_cnt = 0;
		sb_rec_oper = 1;
	}
}

/* port 1 interrupt service routine */
__attribute__((interrupt(PORT1_VECTOR)))
/* cppcheck-suppress unusedFunction */
void p1_isr(void)
{
	uint8_t max_ind;

	/* maximum index value in current screen */
	if (c_scr == 3)
		max_ind = sizeof(dt_curs)/sizeof(uint8_t) - 1;
	else if (c_scr == 5)
		max_ind = sizeof(t_curs)/sizeof(uint8_t) - 1;
	else
		max_ind = 0;

	__delay_ms(300);	/* to avoid bouncing */

	/* this section avoids screen #1 due to it's absence */
	if (((P1IFG & UP_B) || (P1IFG & DOWN_B)) && (c_scr == 0)) {
		P1IFG = P1IFG & ~UP_B;
		P1IFG = P1IFG & ~DOWN_B;
		goto p1_isr_end;
	}
	/* screen navigation */
	if (P1IFG & PREV_B) {
		if (c_scr%2 == 0)
			c_scr = (c_scr == HT_SCR)?(T_SCR):(c_scr - 2);
		else {
			if (c_ind == 0) {
				c_scr -= 1;
				lcd_wr_instr(_display_on);
			}
			else
				c_ind -= 1;
		}
		P1IFG = P1IFG & ~PREV_B;
	}
	else if (P1IFG & NEXT_B) {
		if (c_scr%2 == 0)
			c_scr = (c_scr == T_SCR)?(HT_SCR):(c_scr + 2);
		else {
			if (c_ind == max_ind) {
				c_scr -= 1;
				lcd_wr_instr(_display_on);
			}
			else
				c_ind += 1;
		}
		P1IFG = P1IFG & ~NEXT_B;
	}
	else if (P1IFG & UP_B) {
		/* go to corresponding setup screen */
		if (c_scr%2 == 0) {
			c_scr += 1;
			c_ind = 0;
			lcd_wr_instr(_cursor_on);
		}
		else if (c_scr == DTS_SCR) {
			*(dt_ptr[c_ind]) += 1;	/* increment parameter */
			dt_ch = 1;
		}
		else if (c_scr == TS_SCR) {
			*(t_ptr[c_ind]) += 1;	/* increment parameter */
			dt_ch = 2;
		}

		P1IFG = P1IFG & ~UP_B;
	}
	else if (P1IFG & DOWN_B) {
		/* go to corresponding setup screen */
		if (c_scr%2 == 0) {
			c_scr += 1;
			c_ind = 0;
			lcd_wr_instr(_cursor_on);
		}
		else if (c_scr == DTS_SCR) {
			*(dt_ptr[c_ind]) -= 1;	/* decrement parameter */
			dt_ch = 1;
		}
		else if (c_scr == TS_SCR) {
			*(t_ptr[c_ind]) -= 1;	/* decrement parameter */
			dt_ch = 2;
		}
		P1IFG = P1IFG & ~DOWN_B;
	}
p1_isr_end:
	;
}

