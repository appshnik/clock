#ifndef INTC_H
#define INTC_H

#include <stdint.h>

typedef void (*isr_cb_t)(void *priv);
/* initializes callback isr_cb by referencing to cb with parameters priv */
void intc_p1_add_cb(uint8_t pin, isr_cb_t cb, void *priv);
void intc_p2_add_cb(uint8_t pin, isr_cb_t cb, void *priv);

#endif /* INTC_H */
