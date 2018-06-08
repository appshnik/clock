#include <common.h>
#include <one_wire.h>
#include <sb.h>

struct hutemp ht_data;		/* humidity and temperature data */

void sb_receive(void)
{
	int *p_int = NULL;
	uint8_t *p_uint8 = NULL;
	bool valid_flag = 0;
	int sign = 0;

	p_int = (int*)one_wire_read_data(40);

	if (p_int != NULL) {
		p_uint8 = (uint8_t*)p_int;
		valid_flag = ((uint8_t)(*(p_uint8) + \
					*(p_uint8+1) + \
					*(p_uint8+2) + \
					*(p_uint8+3))) ==\
					((uint8_t)(*(p_uint8+4)));
	}
/*8888888888888888888888*/
	valid_flag = 1;
/*8888888888888888888888*/
	if (valid_flag) {
		ht_data.hum = *p_int;
		sign = (*(p_uint8 + 2) & 0x80) ? (-1) : (1);
		ht_data.temp = (*(p_int + 1) & 0x7FFF) * sign;
	}
}
