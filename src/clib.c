#include <clib.h>
#include <lcd.h>
#include <sb.h>


/* global variables */
char top_str[STR_LEN];	/* top string of LCD */
char bot_str[STR_LEN];	/* bottom string of LCD */
uint8_t c_scr;		/* current screen number */
uint8_t c_ind;		/* current index */
struct date date;	/* current date */
struct time time;	/* current time */
struct timer timer;	/* current timer values */

/* external variables */
extern uint8_t rec_oper;
extern uint8_t bit_count;
/* extern struct hutemp ht_data; */
extern uint16_t tar_val;
/* extern signed char ht_res; */

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

	SB_SEL &= ~SB_MSK;
	SB_SEL2 &= ~SB_MSK;

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
void wr_data(signed char bit, uint8_t *byte)
{
	if (bit)
		*byte |= 1;
	else
		*byte &= ~1;
	/* shift byte for next writing */
	if (bit_count % 8 < 7)
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
				wr_data(bit, &(ht_data.hum_h));
				break;
			case 1	:
				wr_data(bit, &(ht_data.hum_l));
				break;
			case 2	:
				wr_data(bit, &(ht_data.temp_h));
				break;
			case 3	:
				wr_data(bit, &(ht_data.temp_l));
				break;
			case 4	:
				wr_data(bit, &(ht_data.ch_sum));
				break;
			}
		}
		bit_count++;
		if ((bit_count >= 40) || (bit < 0)) {
			bit_count = 0;
			SB_IE &= ~SB_MSK;
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

	max_ind = 4;		/* maximum index value in current screen */

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
			c_scr = (c_scr == 0)?(4):(c_scr - 2);
		else {
			if (c_ind == 0)
				c_scr -= 1;
			else
				c_ind -= 1;
		}
		P1IFG = P1IFG & ~PREV_B;
	}
	else if (P1IFG & NEXT_B) {
		if (c_scr%2 == 0)
			c_scr = (c_scr == 4)?(0):(c_scr + 2);
		else {
			if (c_ind == max_ind)
				c_scr -= 1;
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
		}
		else {
			/*TODO*/;	/* increment parameter */
		}
		P1IFG = P1IFG & ~UP_B;
	}
	else if (P1IFG & DOWN_B) {
		/* go to corresponding setup screen */
		if (c_scr%2 == 0) {
			c_scr += 1;
			c_ind = 0;
		}
		else {
			/*TODO*/;	/* decrement parameter */
		}
		P1IFG = P1IFG & ~DOWN_B;
	}
p1_isr_end:
	;
}

