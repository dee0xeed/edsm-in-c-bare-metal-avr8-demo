
#include <stdlib.h>

#include "bug.h"
#include "edsm.h"
#include "usart.h"
#include "shell.h"
#include "message-queue.h"

struct edsm state_machines[] = {

	[M0_LED] = {
		.name = "led",
		.state_number = 0,
		.states = led_states,
		.data = NULL
	},

	[M1_USART] = {
		.name = "usart",
		.state_number = 0,
		.states = usart_states,
//		.nstates = usart_n_states,
		.data = NULL
	},

	[M2_SHELL] = {
		.name = "shell",
		.state_number = 0,
		.states = shell_states,
		.data = NULL
	}
};

#define N_SM ARRAY_SIZE(state_machines)

void start_state_machines(void)
{
	struct edsm *sm;
	struct state *st;
	    u8 k;

	// TODO: check if N_SM exceeds message buffer capacity...
	for (k = 0; k < N_SM; k++) {

		sm = &state_machines[k];
		if (NULL == sm)
			continue;

		st = &sm->states[0];
		if (st->enter)
			st->enter(sm);
	}
}

void process_messages(void)
{
	struct edsm *sm;
	struct state *state;
	struct reflex *r;
//	action action;
	u8 msg;

	for (;;) {

		msg = dequeue_message();
		if (0xFF == msg)
			/* message queue is empty */
			break;

		/* dispatch the message */
		sm = &state_machines[MSG_DEST_SM(msg)];
		if (NULL == sm)
			bug();
		state = &sm->states[sm->state_number];
		r = &state->reflexes[MSG_EV_CODE(msg)];
		if (NULL == r)
			bug();
		if (r->action)
			r->action(sm);

		if (0xFF == r->next_state)
			/* do not leave the state */
			continue;

		/* make a transition */
		struct state *ns = &sm->states[r->next_state];
		if (ns->enter)
			ns->enter(sm);
		sm->state_number = r->next_state;
	}
}
