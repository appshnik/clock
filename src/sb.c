#include <ow.h>
#include <sb.h>

#define INT_BITS	16	/* bits in int variable */
#define UINT_BITS	8	/* bits in uint8_t variable */

static struct hutemp ht_data;		/* humidity and temperature data */

/**
 * Write next less significant bit to the target
 * @param tar_size	Target field length (in bits)
 * @param target	Pointer to the memory to be written to
 * @param bit_cnt	Number of already written bits (cycle counter)
 * @param bit		Value to be written to the target
 */
void wr_next_bit(int tar_size, void *target, int bit_cnt, bool bit)
{
	void *p = target;
	/* write less significant bit in target */
	if (bit) {
		if (tar_size == 16)
			*((uint16_t *)p) |= 1;
		else
			*((uint8_t *)p) |= 1;
	} else {
		if (tar_size == 16)
			*((uint16_t *)p) &= ~1;
		else
			*((uint8_t *)p) &= ~1;
	}
	/* prepare target for next writing by shifting it */
	if (bit_cnt % tar_size < (tar_size - 1)) {
		if (tar_size == 16)
			*((uint16_t *)p) <<= 1;
		else
			*((uint8_t *)p) <<= 1;
	}
}

struct hutemp *sb_receive(uint8_t *port, uint8_t pin, uint16_t bit_number)
{
	bool *s_buf = NULL;	/* pointer to sensor data buffer */
	uint8_t *p_byte = NULL;
	int i;
	int valid = 0;
	int sign = 0;

	s_buf = ow_read_data(port, pin, bit_number);

	/* Parse data according to hutemp structure */
	if (s_buf != NULL) {
		/* get humidity value */
		for (i = 0; i < INT_BITS; i++)
			wr_next_bit(INT_BITS, \
				    (void *)(&(ht_data.hum)), \
				    i, \
				    *(s_buf + i));

		/* get temperature value */
		for (i = 0; i < INT_BITS; i++)
			wr_next_bit(INT_BITS, \
				    (void *)(&(ht_data.temp)), \
				    i, \
				    *((s_buf + INT_BITS) + i));

		/* get checksum */
		for (i = 0; i < UINT_BITS; i++)
			wr_next_bit(UINT_BITS, \
				    (void *)(&(ht_data.vld)), \
				    i, \
				    *((s_buf + 2*INT_BITS) + i));

		/* check for data validity */
		p_byte = (uint8_t *)(&ht_data);
		valid = ((uint8_t)(*(p_byte) + *(p_byte+1) + \
			  *(p_byte+2) + *(p_byte+3))) == \
			 ((uint8_t)(*(p_byte+4)));


	}

	if (valid) {
		p_byte = (uint8_t *)(&(ht_data.temp));
		sign = (ht_data.temp & 0x8000) ? (-1) : (1);
		ht_data.temp &= 0x7FFF;
		ht_data.temp *= sign;
		/* define temperature sign */
/*		p_byte = (uint8_t *)(&(ht_data.temp));
		sign = (*(p_byte) & 0x80) ? (-1) : (1);
		*(p_byte) &= 0x7F;
		ht_data.temp *= sign;
*/
		return &ht_data;
	} else {
		return NULL;
	}
}
