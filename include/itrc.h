#ifndef INTC_H
#define INTC_H

typedef void (*isr_cb_t)(void *priv);

void intc_add_callback(isr_cb_t cb, int pin, void *priv);

#endif /* INTC_H */
