#include <msp430.h> 

#define TOGGLE_GREEN	P1DIR |= (1<<6), P1OUT ^= (1<<6)
#define TOGGLE_RED	P1DIR |= (1<<7), P1OUT ^= (1<<7)
#define _wait(x)	for (i = 0; i < (x); i++)

extern int c_mode;

int main(void)
{
	unsigned long i;
	/* initialization */
	WDTCTL = WDTPW | WDTHOLD;
	P1SEL = 0x00;
	P1SEL2 = 0x00;
	TOGGLE_GREEN;
	/* main program loop */
	while (1) {
		TOGGLE_GREEN;
		_wait(30000UL);
		TOGGLE_RED;
		_wait(30000UL);
		TOGGLE_RED;
		_wait(30000UL);
	}
	return 0;
}
