// #include <msp430.h>
#include "clib.h"

/* global variables */
int year, month, day, hour, min, sec;
int c_mode;	// current clock mode

/* desable WDT */
void wdt_init(void)
{
	WDTCTL = WDTCTL | WDTHOLD;
}

/* ports 1 and 2 initialization */
void gpio_init(void)
{
	P2DIR = 0xF0; // port 2 pins 0, 1, 2, 3 are configured as inputs
	P2IES = 0x0F; // high-low transition causes interrupt
	P2IE  = 0x0F; // buttons interrupts are enabled

	P1DIR = 0xFF; // port 1 pins are configured as outputs
}

/* variables init */
//TODO
/* MCLK init */
void mclk_init(void)
{
	BCSCTL2 = DIVM_0 | SELM_3;
	BCSCTL3 = LFXT1S_2;
}
/* initializaton routine */
void init_device()
{
	__enable_interrupt();
	wdt_init();
	gpio_init();
	//var_init();
	//mclk_init();
}

/* port 2 interrupt service routine */
__attribute__((interrupt(PORT1_VECTOR)))
void keyboard_isr(void)
{	
	/* change clock mode */
	if (c_mode == MAIN_MD) {
		c_mode = SEL_MD;
	}
	else if (c_mode == SEL_MD) {
		;
	}
	/* start setting timer */
	//TODO
	/* process "set" button event */
	P2IFG = P2IFG & (~0x0F);

}

