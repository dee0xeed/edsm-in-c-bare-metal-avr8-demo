
/* Event Driven State Machines for AVR8 */

#ifndef AVR8_EDSM_H
#define AVR8_EDSM_H

#include "itypes.h"

struct edsm;
struct state;
struct reflex;

typedef void (*action)(struct edsm *sm);

struct edsm {
	  char *name; // ?
	    u8 state_number;	/* number of the current state of the machine */
	struct state *states;
//	    u8 nstates;
	  void *data;
};

struct state {
	  char *name; // ?
	action enter;
	struct reflex *reflexes;
	    u8 n_reflexes;
};

struct reflex {
	action action;
	    u8 next_state;
};

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

/* state machine numbers */
#define M0_LED		0
#define M1_USART	1
#define M2_SHELL	2

extern struct state led_states[];
extern struct state usart_states[];
extern struct state shell_states[];

void start_state_machines(void);
void process_messages(void);

/* generic 'names' for reflexes */
#define R0_		0
#define R1_		1
#define R2_		2
#define R3_		3
#define R4_		4
#define R5_		5
#define R6_		6
#define R7_		7
#define R8_		8
#define R9_		9

/* generic 'names' for states */
#define S0_		0
#define S1_		1
#define S2_		2
#define S3_		3
#define S4_		4
#define S5_		5
#define S6_		6
#define S7_		7
#define S8_		8
#define S9_		9

#endif
