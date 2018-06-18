#include <common.h>
#include <ow.h>

#ifdef OUT
#undef OUT
#endif

#ifndef CPU_F
#define CPU_F		20000000UL		/* oscillator frequency */
#define TA_FAC		(CPU_F/1000000UL)	/* ticks per microsecond */
/**
 * IMPORTANT!: CPU frequency must be greater or equal to 10 MHz otherwise
 * __delay_us(us) macro will be equal to 0. If you use CPU_F less then 10 MHz
 * you should modify this macro or just substitute it by __delay_ms(ms) macro
 * (it will slow process down a little).
 */

/* function macros */
#define __delay_ms(ms)	(__delay_cycles((unsigned long)(CPU_F/1000UL * ms)))
#define __delay_us(us)	(__delay_cycles((unsigned long)(CPU_F/1000000UL * us)))

#endif /* CPU_F */

#define MAX_RESP_TIME	32000
#define R_LOW_MIN_TIME	75	/* us */
#define R_LOW_MAX_TIME	85	/* us */
#define R_HGH_MIN_TIME	75	/* us */
#define R_HGH_MAX_TIME	85	/* us */
#define ONE_MIN_TIME	55	/* us */
#define ONE_MAX_TIME	75	/* us */
#define ZERO_MIN_TIME	10	/* us */
#define ZERO_MAX_TIME	30	/* us */

static volatile uint16_t ow_bit_count;	/* amount of already received bits */
static volatile uint16_t ow_bit_number;	/* amount of bits to receive */
static bool ow_buffer[8*5];	/* buffer for received data */

int flag = 0;

/* interrupt service routine for SB pin */
/**
  * This ISR serves to calculate time periods of 1's and 0's
  * during data transition through single-bus interface. ISR starts
  * with low-high transition and counts time period. After signal's
  * high-low transition it checks whether time period corresponds to "1"
  * or "0" and writes result to corresponding destination.
  */

/* main idea */
/**
  * User's function initiates data reading through single-bus interface.
  * After start signal it disables any interrupts on this port and enables
  * the one that belongs to desired port pin. Function starts a cycle which is
  * to be performed while data transfer isn't finished (some flag is switched).
  * Then interrupts settings are restored (all data receiving/storing job is
  * implemented in ISR).
  */

/* Generates START condition on the bus */
void ow_start(uint8_t *port, uint8_t pin)
{
	/* set pin direction as output: register PxDIR */
	*(port + 2) |= (1 << pin);
	/* set pin state to 0: register PxOUT */
	*(port + 1) &= ~(1 << pin);
	__delay_ms(25);
	/* set pin state to 1: register PxOUT */
	*(port + 1) |= (1 << pin);
	__delay_us(30);
	/* set pin direction as input: register PxDIR */
	*(port + 2) &= ~(1 << pin);
	/* clear PxOUT register pin */
	*(port + 1) &= ~(1 << pin);
}

uint8_t ow_resp(uint8_t *port, uint8_t pin)
{
	unsigned char res = 1;
	int i = 0;

	/* wait until sensor responses "low": reading register PxIN */
	i = MAX_RESP_TIME;
	while ((*(port + 0) & (1 << pin)) && i--)
		flag = 0;
	if (i <= 0)
		return 0;
	/* wait until sensor responses "high": reading register PxIN */
	i = MAX_RESP_TIME;
	while (!(*(port + 0) & (1 << pin)) && i--)
		flag = 0;
	if (i <= 0)
		return 0;
	/* wait until sensor starts data transfer: reading register PxIN */
	i = MAX_RESP_TIME;
	while ((*(port + 0) & (1 << pin)) && i--)
		flag = 0;
	if (i <= 0)
		return 0;

	return res;
}

/**
 * Function is used to receive next bit from sensor. It is called in port ISR.
 */
void ow_get_bit(void *priv)
{
	port_pin *pp = priv;
	signed char val = 0;
	uint16_t tar_val = 0;	/* microseconds expired */

	/* set TimerA to zero */
	TACTL |= TACLR & ~TAIE;
	/* wait until high-low transition: reading register PxIN */
	if (ow_bit_count < ow_bit_number)
		while ((*(pp->port + 0) & (1 << pp->pin)))
			flag = 0;

	/* compare TAR with predefined values and return corresponding value */
	tar_val = (unsigned int)(TAR) / TA_FAC;

	if (tar_val >= ONE_MIN_TIME && tar_val <= ONE_MAX_TIME)
		val = 1;
	else if (tar_val >= ZERO_MIN_TIME && tar_val <= ZERO_MAX_TIME)
		val = 0;
	else
		val = -1;
	/* ignore first short impulse from sensor */
	if (val < 0 && ow_bit_count == 0)
		goto _end;
	if (val >= 0) {
		ow_buffer[ow_bit_count++] = val;
	}
_end:
	;
}

/* Data receiving routine */
bool *ow_read_data(uint8_t *port, uint8_t pin, uint16_t bit_number)
{
	struct pie {		/* structure to save previous settings */
		uint8_t p1;
		uint8_t p2;
		uint8_t wdt;
		uint16_t t0a;
		uint16_t t1a;
	} pie;

	__disable_interrupt();
	/* save current values and turn off all interrupt enable flags */
	pie.p1 = P1IE;
	P1IE = 0x00;
	pie.p2 = P2IE;
	P2IE = 0x00;
	pie.wdt = IE1;
	IE1 &= ~WDTIE;
	pie.t0a = TA0CTL;
	TA0CTL &= ~TAIE;
	pie.t1a = TA1CTL;
	TA1CTL &= ~TAIE;

	ow_bit_count = 0;
	ow_bit_number = bit_number;
	/* set port interrupt enable flag: register PxIE */
	*(port + 5) |= (1 << pin);
	/* set port interrupt edge (low->high): register PxIES */
	*(port + 4) &= ~(1 << pin);

	/* send start signal to sensor */
	ow_start(port, pin);
	/* check if sensor has responded */
	flag = ow_resp(port, pin);
	if (flag) {
		/* start bit reading sequence */
		__enable_interrupt();
		while (ow_bit_count < bit_number) {
			flag = 1;
		}
	}

	/* restore all settings */
	__disable_interrupt();
	P1IE = pie.p1;
	P2IE = pie.p2;
	IE1 = pie.wdt;
	TA0CTL = pie.t0a;
	TA1CTL = pie.t1a;
	__enable_interrupt();

	if (flag)
		return ow_buffer;
	else
		return NULL;
}
