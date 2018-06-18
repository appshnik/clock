#ifndef ONE_WIRE_H
#define ONE_WIRE_H

#include <msp430.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

/* one wire bus received data buffer */
typedef bool (*ow_buf)[];

/* function definitions */
void ow_get_bit(void *priv);
bool *ow_read_data(uint8_t *port, uint8_t bit, uint16_t bit_number);

#endif /* ONE_WIRE_H */
