#include "clib.h"
#include "lcd_drv.h"
#include "sb_drv.h"


/* global variables */
char top_str[STR_LEN];	// top string of LCD
char bot_str[STR_LEN];	// bottom string of LCD
int c_mode;		// current clock mode

/* external variables */
extern uint8_t rec_oper;
extern uint8_t bit_count;
extern struct hutemp ht_data;
extern uint16_t tar_val;

/* desable WDT */
void wdt_init(void)
{
	WDTCTL = WDTPW | WDTHOLD;
}

/* ports 1 and 2 initialization */
void gpio_init(void)
{
//	P1IES = 0x0F; // high-low transition causes interrupt
//	P1IE  = 0x0F; // buttons interrupts are enabled

	P2IES = 0x00;
	P2IE = 0x00;
}
/* Timer A initialization */
void ta_init(void)
{
	TACTL = TASSEL_2;
	TACTL |= MC_2; 
}

/* variables init */
//TODO
/* MCLK init as 20 MHz*/
void mclk_init(void)
{
	DCOCTL = 0xE0;			// DCOx = 7	20 MHz
	BCSCTL1 = BCSCTL1 | 0x0F;	// RSELx = 15
//	DCOCTL = 0x60;			// DCOx = 3	1 MHz
//	BCSCTL1 = BCSCTL1 | 0x07;	// RSELx = 7
	
	
	
}
/* initializaton routine */
void init_device(void)
{
	__enable_interrupt();
	wdt_init();
	mclk_init();
	gpio_init();
	ta_init();
	//var_init();
}

void wr_data(signed char bit, uint8_t *byte) {
	if (bit)
		*byte |= 1;
	else
		*byte &= ~1;

	if (bit_count % 8 < 7)	
		*byte = *byte << 1;
}

/* port 2 interrupt service routine */
__attribute__((interrupt(PORT2_VECTOR)))
void p2_isr(void)
{
	/* temperature and humidity data reading */
	signed char bit;
	if (P2IFG & SB_MSK) {
		if (bit_count < 40) {
			bit = sb_get_bit();
		}
//		else
//			bit = -1;

		//hernya		
		if (bit < 0 && bit_count == 0)
			goto isr_end;

		bit_count++;

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

		if ((bit_count >= 40) || (bit < 0)) {
			sprintf(top_str, "%d_%d", tar_val, bit_count);
			wr_str(top_str, 0x00);
			bit_count = 0;
			rec_oper = 0;
			SB_IE &= ~SB_MSK;
		}
isr_end:
		P2IFG = P2IFG & ~SB_MSK;
	}
}

