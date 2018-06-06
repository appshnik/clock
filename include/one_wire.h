#ifndef ONE_WIRE_H
#define ONE_WIRE_H

#include <msp430.h>
#include <stdint.h>

/* function definitions */
void one_wire_start(void);
uint8_t one_wire_resp(void);
signed char one_wire_receive(void);
signed char one_wire_get_bit(void);
signed char one_wire_read_data(void);

/* variables definitions*/
extern signed char ht_res;

#endif /* ONE_WIRE_H */
