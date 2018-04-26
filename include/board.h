#ifndef BOARD_H
#define BOARD_H

#define HT_SCR		(0)			/* humidity/temperature screen*/
#define DT_SCR		(2)			/* d/t screen number */
#define T_SCR		(4)			/* timer screen number */
#define DTS_SCR		(3)			/* d/t set screen number */
#define TS_SCR		(5)			/* timer set screen number */

/* function definitions */
void init_device(void);

/* external variables */
extern uint8_t c_scr;
extern uint8_t c_ind;
extern uint8_t dt_curs[];
extern uint8_t t_curs[];
extern uint8_t bit_count;

#endif /* BOARD_H */
