#include <common.h>
#include <itrc.h>
#include <one_wire.h>

/* XXX: Move to common.h */
#define BIT(n)		(1 << n)

#define CB_LIST_LEN	8

struct intc_cb {
	isr_cb_t cb;
	void *priv;
};

static struct intc_cb p1_list[CB_LIST_LEN];

/**
 * Add callback for specified pin to INTC table.
 * @param cb Callback function
 * @param pin Pin number (0-7)
 * @param priv Private data for callback
 */
void intc_add_callback(isr_cb_t cb, int pin, void *priv)
{
	struct intc_cb cb;

	assert(pin < CB_LIST_LEN);

	cb.cb = cb;
	cb.priv = priv;

	cb_list[pin] = cb;
}

__attribute__((interrupt(PORT1_VECTOR)))
void p1_isr(void)
{
	int i;

	for (i = 0; i < CB_LIST_LEN; ++i) {
		if (P1IFG & BIT(i) && p1_list[i].cb != NULL)
			p1_list[i].cb(p1_list[i].priv);
	}

	P1IFG = 0x00;
}
