
#ifndef USART_H
#define USART_H

#include "itypes.h"
#include "edsm.h"

struct usart_txbuf {
	u8 *pos;
	u8  cnt;
};

struct usart_rxbuf {
	u8 *loc;
	u8  cnt;
};

/* state numbers */

#define S0_USART_INIT	0
#define S1_USART_RX	1
#define S2_USART_TX	2

/* reflex numbers */

#define R0_DRE		R0_
#define R0_RXC		R0_

#define RXB_SIZE 32
#define RXB_MASK (RXB_SIZE - 1)

/* 'getters' */
struct usart_rxbuf *usart_rxbuf(void);
struct usart_txbuf *usart_txbuf(void);

/* 'setters' */
  u8 usart_init_txbuf(u8 *str, u8 len);
void usart_init_rxbuf();

#endif
