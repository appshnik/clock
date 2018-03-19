#include "clib.h"

extern int year, month, day, hour, min, sec;
extern int c_mode;	// current clock mode

__enable_interrupt();

/* desable WDT */
void wdt_init(void)
{
	WCTCTL = WDTCTL | WDTHOLD;
}

/* port 2 initialization */
void gpio_init(void)
{
	P2DIR = 0xF0; // port 2 pins are configured as inputs
	P2IES = 0x0F; // high-low transition causes interrupt
	P2IE  = 0x0F; // buttons interrupts are enabled
}

/* variables init */
//TODO
/* MCLK init */
//TODO

/* port 2 interrupt service routine */
__attribute__((interrupt(PORT1_VECTOR)))
void keyboard_isr(void)
{	
	/* change clock mode */
	if (c_mode == MAIN_MD) {
		c_mode = SEL_MD;
	}
	else if (c_mode == SEL_MD && ) {
		;
	}
	/* start setting timer */
	//TODO
	/* process "set" button event */
	P2IFG = P2IFG & (~0x0F);

}

