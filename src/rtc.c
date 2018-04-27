#include <common.h>
/* #include <rtc.h> */

#define RTC_ADR		(0x68)		/* RTC modules I2C bus address */

int bcd_nybble_to_int(uint8_t bcd)
{
	int value;
	uint8_t nybble = bcd & 0x0F;

	value = 8 * (nybble & 0x08) + \
		4 * (nybble & 0x04) + \
		2 * (nybble & 0x02) + \
		1 * (nybble & 0x01);

	return value;
}

int bcd_to_int(uint8_t bcd)
{
	int value;
	uint8_t msn = (bcd >> 4) & 0x0F;
	uint8_t lsn = bcd & 0x0F;

	value = 10 * bcd_nybble_to_int(msn) + \
		bcd_nybble_to_int(lsn);

	return value;
}

#if 0
I2C data write to RTC module:
	master initiates START -->
	master sends RTC_ADR + (lsb = 0) -->
	slave responds -->
	master sends register (to be written) pointer -->
	slave responds -->
	master sends some amount of bytes (register pointer increments
		automatically) -->
	master initiates STOP

I2C data read from RTC module:
	master initiates START -->
	master sends RTC_ADR + (lsb = 0) -->
	slave responds -->
	master sends register (to be read) pointer -->
	master initiates RESTART -->
	master sends RTC_ADR + (lsb = 1) -->
	slave responds -->
	slave starts to transmit data from register pointer address and
		further while it receives acknowledgment from master
		(register pointer is automatically incremented)
#endif








