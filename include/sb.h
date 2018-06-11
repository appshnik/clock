#ifndef SB_H
#define SB_H

#include <stdint.h>

struct hutemp {
	uint8_t hum_h;		/* humidity high byte */
	uint8_t hum_l;		/* humidity low byte */
	uint8_t temp_h;		/* temperature high byte */
	uint8_t temp_l;		/* temperature low byte */
	uint8_t ch_sum;		/* check sum */
	int hum;		/* current humidity */
	int temp;		/* current humidity */
};

/* function definitions */
struct hutemp *sb_receive(void);

#endif /* SB_H */
