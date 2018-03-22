// #include <msp430.h>
#include "clib.h"
#include "lcd_drv.h"

/* global variables */
char top_str[STR_LEN];	// top string of LCD
char bot_str[STR_LEN];	// bottom string of LCD
int c_mode;		// current clock mode

/* desable WDT */
void wdt_init(void)
{
	WDTCTL = WDTPW | WDTHOLD;
}

/* ports 1 and 2 initialization */
void gpio_init(void)
{
	P2DIR = 0xF0; // port 2 pins 0, 1, 2, 3 are configured as inputs
	P2OUT = 0x00;
	P2IES = 0x0F; // high-low transition causes interrupt
	P2IE  = 0x0F; // buttons interrupts are enabled

	P1DIR = 0xFF; // port 1 pins are configured as outputs
	P1OUT = 0x00;
}

/* variables init */
//TODO
/* MCLK init */
void mclk_init(void)
{
	DCOCTL = (DCOCTL | DCO0 | DCO1) & ~(DCO2);
	BCSCTL1 = (BCSCTL1 | RSEL3) & ~(RSEL0) & ~(RSEL1) & ~(RSEL2);
	BCSCTL2 = DIVM_0 | SELM_0;
}
/* initializaton routine */
void init_device(void)
{
//	__enable_interrupt();
	wdt_init();
	mclk_init();
	gpio_init();
	//var_init();
}

/* port 2 interrupt service routine */
__attribute__((interrupt(PORT2_VECTOR)))
void keyboard_isr(void)
{	
	/* change clock mode */
	if (c_mode == MAIN_MD) {
		c_mode = SEL_MD;
	}
	else if (c_mode == SEL_MD) {
		;//TODO
	}
	/* start setting timer */
	//TODO
	/* process "set" button event */
	//TODO
	P2IFG = P2IFG & (~0x0F);

}

