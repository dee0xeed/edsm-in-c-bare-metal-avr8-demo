
/* Command Interpreter State Machine */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "shell.h"
#include "edsm.h"
#include "message-queue.h"
#include "usart.h"

void shell_init_enter(struct edsm *me);

void shell_wait_enter(struct edsm *me);
void shell_wait_R0(struct edsm *me);

void shell_errr_enter(struct edsm *me);
void shell_rcfg_enter(struct edsm *me);
void shell_wcfg_enter(struct edsm *me);
void shell_repl_enter(struct edsm *me);

struct reflex shell_init_reflexes[] = {

	[R0_] = {
		.action = NULL,
		.next_state = S1_SHELL_WAIT,
	},
};

struct reflex shell_wait_reflexes[] = {

	[R0_] = { /* got command */
		.action = shell_wait_R0,
		.next_state = 0xFF,
	},

	[R1_] = { /* parse error  */
		.action = NULL,
		.next_state = S2_SHELL_ERRR,
	},

	[R2_] = { /* read config */
		.action = NULL,
		.next_state = S3_SHELL_RCFG,
	},

	[R3_] = { /* save config */
		.action = NULL,
		.next_state = S4_SHELL_WCFG,
	},
};

struct reflex shell_errr_reflexes[] = {

	[R0_] = {
		.action = NULL,
		.next_state = S5_SHELL_REPL,
	},
};

struct reflex shell_rcfg_reflexes[] = {

	[R0_] = {
		.action = NULL,
		.next_state = S5_SHELL_REPL,
	},

	[R1_] = {
		.action = NULL,
		.next_state = S2_SHELL_ERRR,
	},
};

struct reflex shell_wcfg_reflexes[] = {

	[R0_] = {
		.action = NULL,
		.next_state = S5_SHELL_REPL,
	},

	[R1_] = {
		.action = NULL,
		.next_state = S2_SHELL_ERRR,
	},
};

struct reflex shell_repl_reflexes[] = {

	[R0_] = {
		.action = NULL,
		.next_state = S1_SHELL_WAIT,
	},
};

struct state shell_states[] = {

	[S0_SHELL_INIT] = {
		.name = "init",
		.enter = shell_init_enter,
		.reflexes = shell_init_reflexes,
		.n_reflexes = ARRAY_SIZE(shell_init_reflexes),
	},

	[S1_SHELL_WAIT] = {
		.name = "wait",
		.enter = shell_wait_enter,
		.reflexes = shell_wait_reflexes,
		.n_reflexes = ARRAY_SIZE(shell_wait_reflexes),
	},

	[S2_SHELL_ERRR] = {
		.name = "errr",
		.enter = shell_errr_enter,
		.reflexes = shell_errr_reflexes,
		.n_reflexes = ARRAY_SIZE(shell_errr_reflexes),
	},

	[S3_SHELL_RCFG] = {
		.name = "rcfg",
		.enter = shell_rcfg_enter,
		.reflexes = shell_rcfg_reflexes,
		.n_reflexes = ARRAY_SIZE(shell_rcfg_reflexes),
	},

	[S4_SHELL_WCFG] = {
		.name = "wcfg",
		.enter = shell_wcfg_enter,
		.reflexes = shell_wcfg_reflexes,
		.n_reflexes = ARRAY_SIZE(shell_wcfg_reflexes),
	},

	[S5_SHELL_REPL] = {
		.name = "repl",
		.enter = shell_repl_enter,
		.reflexes = shell_repl_reflexes,
		.n_reflexes = ARRAY_SIZE(shell_repl_reflexes),
	},
};

struct shell_data my_data;

void shell_init_enter(struct edsm *me)
{
	struct usart_rxbuf *rxb = usart_rxbuf();
	my_data.rxb = rxb->loc;
	me->data = &my_data;

	enqueue_message_sw(M2_SHELL, R0_);
}

void shell_wait_enter(struct edsm *me)
{
	/* do nothing */
}

void shell_wait_R0(struct edsm *me)
{
	struct shell_data *d = me->data;
	    u8 *rxb = d->rxb;
	    u8 cmd = *rxb;
	    u8 rc = 0;

	switch (cmd) {

	case 'r': /* read condig */
		rc = R2_;
	break;

	case 'c': /* save config */
		rc = R3_;
	break;

	default:
		rc = R1_; /* unknown command */
	}

	enqueue_message_sw(M2_SHELL, rc);
}

void shell_errr_enter(struct edsm *me)
{
	struct shell_data *d = me->data;
	    u8 *txb = d->txb;
	    u8 *rxb = d->rxb;

	/* unsupported command */
	sprintf((char*)txb, "'%s': error\r\n", (char*)rxb);
	enqueue_message_sw(M2_SHELL, R0_);
}

void shell_rcfg_enter(struct edsm *me)
{
	struct shell_data *d = me->data;
	    u8 *txb = d->txb;
	    u8 *rxb = d->rxb;
	   u16 addr;
	   int r;
//	    u8 b;

	r = sscanf((char*)rxb, "r %d", &addr);
	if (1 != r) {
		enqueue_message_sw(M2_SHELL, R1_);
		return;
	}

	//b = eeprom_get_byte(addr);
	//sprintf(txb, "%.2X\r\n", b);
	sprintf((char*)txb, "RR\r\n");
	enqueue_message_sw(M2_SHELL, R0_);
}

void shell_wcfg_enter(struct edsm *me)
{
	//static struct my_config c;
	struct shell_data *d = me->data;
	    u8 *txb = d->txb;
	    u8 *rxb = d->rxb;
	   int r;
	   u8 p1,p2,p3,p4;

	//r = sscanf((char*)rxb, "c %hhd,%hhd,%hhd,%hhd", &c.a, &c.b, &c.c, &c.d);
	r = sscanf((char*)rxb, "c %hhd,%hhd,%hhd,%hhd", &p1, &p2, &p3, &p4);
	if (4 != r) {
		enqueue_message_sw(M2_SHELL, R1_);
		return;
	}

	//eeprom_prep_save((u8*)&c, EEPROM_CONFIG_OFFSET, sizeof(struct my_config));
	sprintf((char*)txb, "WW\r\n");
	enqueue_message_sw(M2_SHELL, R0_);
	return;
}

void shell_repl_enter(struct edsm *me)
{
	struct shell_data *d = me->data;
	    u8 *txb = d->txb;

	usart_init_txbuf(txb, strlen((char*)txb));
	enqueue_message_sw(M1_USART, R1_);
}
