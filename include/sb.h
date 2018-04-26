#ifndef SB_H
#define SB_H

/* function definitions */
void sb_start(void);
uint8_t sb_resp(void);
signed char sb_receive(void);
signed char sb_get_bit(void);
signed char sb_read_data(void);

/* variables definitions*/
extern signed char ht_res;

#endif /* SB_H */
