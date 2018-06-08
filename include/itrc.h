typedef void (*isr_cb_t)(void *param);

struct cb_t {
	isr_cb_t cb;	/* function to be executed */
	void *param;	/* parameters for the function */
};

/* initializes callback isr_cb by referencing to cb with parameters param*/
void set_cb(struct cb_t *isr_cb, isr_cb_t cb, void *param);
void callback(struct cb_t *isr_cb);

extern struct cb_t p2_isr_cb;	/* port 2 ISR callback */
