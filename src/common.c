#include <common.h>

uint8_t bcd_to_int(uint8_t bcd)
{
	int value;
	uint8_t msn = (bcd >> 4) & 0x0F;
	uint8_t lsn = bcd & 0x0F;

	value = 10 * msn + lsn;

	return value;
}

uint8_t int_to_bcd(uint8_t val)
{
	if (val <= 99)
		return ((val / 10) << 4) | (val % 10);
	else
		return 0;
}
