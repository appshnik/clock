#ifndef ONE_WIRE_H
#define ONE_WIRE_H

#include <msp430.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

/* one wire bus received data buffer */
typedef bool (*ow_buf_t)[];

bool *ow_read_data(uint16_t bits);

#endif /* ONE_WIRE_H */
