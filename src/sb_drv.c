#include <sb_drv.h>
#include <stdint.h>

struct hutemp ht_data;		/* humidity and temperature data */
uint8_t rec_oper;		/* data reading is active */
uint8_t bit_count;		/* number of already received bits */

uint16_t tar_val;		/* microseconds */

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
  * Then interrupts settings are restored (all data receiving/storing work is
  * implemented in ISR).
  */

void sb_start(void)
{
	_OCCUPY_BUS;
	_SET_SB_OUT(0);
	_delay_ms(25);
	_SET_SB_OUT(1);
	_delay_us(30);
	_RELEASE_BUS;
}

uint8_t sb_resp(void)
{
	unsigned char res;
	int i;
	res = 1;
	/* wait until sensor responses "low" */
	i = MAX_RESP_TIME;
	while (_SB_STATE && i--);
	if (i <= 0)
		return 0;
	/* wait until sensor responses "high" */
	i = MAX_RESP_TIME;
	while (!_SB_STATE && i--);
	if (i <= 0)
		return 0;
	/* wait until sensor starts data transfer */
	i = MAX_RESP_TIME;
	while (_SB_STATE && i--);
	if (i <= 0)
		return 0;
	return res;
}

signed char sb_receive(void)
{
	uint8_t itr;	/* holder for previous interrupt settings */
	static uint8_t failed;

	itr = SB_IE;
	rec_oper = 1;
	bit_count = 0;

	/* enable sufficient interrupt */
	SB_IE = SB_MSK;
	/* setup TimerA */
	/* TODO */
	/* wait while reading */
	__delay_ms(100);
	/* restore interrupt settings */
	SB_IE = itr;
	/* data validation */
	if ((ht_data.hum_h + ht_data.hum_l + \
		ht_data.temp_h + ht_data.temp_l) != ht_data.ch_sum) {
		ht_data.hum_h = 0x00;
		ht_data.hum_l = 0x00;
		ht_data.temp_h = 0x00;
		ht_data.temp_l = 0x00;
		ht_data.ch_sum = 0x00;
		return ++failed;
	}
	/* calculate temp and humidity data */
	ht_data.hum = ((ht_data.hum_h)<<8 | ht_data.hum_l) & 0x7FFF;
	ht_data.temp = ((ht_data.temp_h)<<8 | ht_data.temp_l) & 0x7FFF;
	return (failed = 0);
}

/**
  * Function is used to receive next bit from sensor. It is called in port ISR.
  */
signed char sb_get_bit(void)
{
	/* set TimerA to zero */
	TACTL |= TACLR;
	/* wait until high-low transition */
	while (SB_IN & SB_MSK);
	/* compare TAR with predefined values and return corresponding value */
	tar_val = (unsigned int)(TAR) / TA_FAC;
	if ( tar_val >= ONE_MIN_TIME && tar_val <= ONE_MAX_TIME)
		return 1;
	else if ( tar_val >= ZERO_MIN_TIME && tar_val <= ZERO_MAX_TIME)
		return 0;
	else
		return -1;
}

/* Data receiving routine */
signed char sb_read_data(void)
{
	/* send start signal to sensor */
	sb_start();
	/* check if sensor has responded */
	if (sb_resp())
		/* start bit reading sequence */
		return sb_receive();
	else
		return -1;
}
