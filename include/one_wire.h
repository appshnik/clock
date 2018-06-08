#ifndef ONE_WIRE_H
#define ONE_WIRE_H

#include <msp430.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

/* one wire bus received data buffer */
typedef bool (*ow_buf)[];

/* function definitions */
void one_wire_start(void);
uint8_t one_wire_resp(void);
signed char one_wire_receive(void);
void one_wire_get_bit(void *param);
bool *one_wire_read_data(uint16_t bit_number);

#endif /* ONE_WIRE_H */
