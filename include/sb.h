#ifndef SB_H
#define SB_H

#include <stdint.h>

struct hutemp {
	int hum;		/* current humidity */
	int temp;		/* current humidity */
	uint8_t vld;		/* checksum */
};

/* function definitions */
struct hutemp *sb_receive(uint8_t *port, uint8_t pin, uint16_t bit_number);

#endif /* SB_H */
