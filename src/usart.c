
/* USART State Machine */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "init.h"
#include "usart.h"
#include "bug.h"
#include "itypes.h"
#include "message-queue.h"
#include "edsm.h"

void usart_init_enter(struct edsm *me);

void usart_rx_enter(struct edsm *me);
void usart_rx_R0(struct edsm *me);
//void usart_rx_R1(struct edsm *me);

void usart_tx_enter(struct edsm *me);
void usart_tx_R0(struct edsm *me);
//void usart_tx_R1(struct edsm *me);

#define SELF M1_USART

struct reflex usart_init_reflexes[] = {

	[0] = {
		.action = NULL,
		.next_state = S1_USART_RX,
	},
};

struct reflex usart_rx_reflexes[] = {

	[R0_RXC] = {
		.action = usart_rx_R0,
		.next_state = 0xFF,
	},

	[R1_] = { /* start transmit reply */
		.action = NULL,
		.next_state = S2_USART_TX,
	},
};

struct reflex usart_tx_reflexes[] = {

	[R0_DRE] = {
		.action = usart_tx_R0,
		.next_state = 0xFF,
	},

	[R1_] = { /* get next command */
		.action = NULL,
		.next_state = S1_USART_RX,
	},
};

struct state usart_states[] = {

	[S0_USART_INIT] = {
		.name = "init",
		.enter = usart_init_enter,
		.reflexes = usart_init_reflexes,
		.n_reflexes = ARRAY_SIZE(usart_init_reflexes)
	},

	[S1_USART_RX] = {
		.name = "rx",
		.enter = usart_rx_enter,
		.reflexes = usart_rx_reflexes,
		.n_reflexes = ARRAY_SIZE(usart_rx_reflexes)
	},

	[S2_USART_TX] = {
		.name = "tx",
		.enter = usart_tx_enter,
		.reflexes = usart_tx_reflexes,
		.n_reflexes = ARRAY_SIZE(usart_tx_reflexes)
	},
};

/* INIT */
void usart_init_enter(struct edsm *me)
{
	//u16 ubrrval = UBRRVAL(brate);
	u16 ubrrval = UBRRVAL(9600);

	UCSR0B = 0x00;		/* disable everything */
	UCSR0A = 0x00;		/* normal */
	UCSR0C = 0x06;		/* async, 8N1 */

	UBRR0H = (u8)(ubrrval >> 8);
	UBRR0L = (u8)(ubrrval);

	enqueue_message_sw(SELF, R0_);
}

/* RX */

static u8 _rxb[RXB_SIZE];
static struct usart_rxbuf rxb = {
	.loc = _rxb,
};

void usart_rx_enter(struct edsm *me)
{
	rxb.cnt = 0;
	/* enable receiver and interrupt on RX complete */
	UCSR0B |= _BV(RXEN0) | _BV(RXCIE0);
}

void usart_rx_R0(struct edsm *me)
{
	u8 s = UDR0;

	if ('\r' == s) {
		/* disable interrupt on RX and receiver itself */
		UCSR0B &= ~(_BV(RXCIE0) | _BV(RXEN0));
		rxb.loc[rxb.cnt] = 0;
		enqueue_message_sw(M2_SHELL, R0_);
		return;
	}

	rxb.loc[rxb.cnt] = s;
	rxb.cnt ++;
	rxb.cnt &= RXB_MASK;

	/* reenable interrupt on RX complete */
	UCSR0B |= _BV(RXCIE0);
}

struct usart_rxbuf *usart_rxbuf(void)
{
	return &rxb;
}

/* TX */

static struct usart_txbuf txb = {0};

struct usart_txbuf *usart_txbuf(void)
{
	return &txb;
}

u8 usart_init_txbuf(u8 *dat, u8 len)
{
	if (txb.cnt) {
		/* tx is in progress already */
		return 1;
	}

	txb.pos = dat;
	txb.cnt = len;
	return 0;
}

void usart_tx_enter(struct edsm *me)
{
	/* enable USART DRE interrupt and transmitter itself */
	UCSR0B |= (_BV(TXEN0) | _BV(UDRIE0));
	return;
}

void usart_tx_R0(struct edsm *me)
{
	if (0 == txb.cnt) {
		/* tx is not in progress */
		bug();
		return;
	}

	UDR0 = *txb.pos;
	txb.cnt--;

	if (0 == txb.cnt) {
		/* transfer complete */
		enqueue_message_sw(SELF, R1_);
		enqueue_message_sw(M2_SHELL, R0_);
		return;
	}

	/* advance the pointer */
	txb.pos++;
	/* and reenable USART DRE interrupt */
	UCSR0B |= _BV(UDRIE0);
}
