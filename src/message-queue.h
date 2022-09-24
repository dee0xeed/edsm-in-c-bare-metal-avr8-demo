
#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include "itypes.h"

/*
 * mailbox - message ring buffer
 * NOTE: MBOX_CAPACITY __MUST__ be the integer power of two
 */

#define MBOX_CAPACITY	16
#define MBOX_MASK	(MBOX_CAPACITY - 1)

struct mailbox {
	u8 msg[MBOX_CAPACITY];
	u8 wri;	/* writer index */
	u8 rdi;	/* reader index */
	u8 num;	/* number of messages in the buffer */
};

/*
 * message queue (2 mailboxes)
 */

struct message_queue {
	struct mailbox mbox_hw;		/* for ISRs */
	struct mailbox mbox_sw;		/* for other code */
};

/* interface */

void init_message_queue(void);
void enqueue_message_hw(u8 dst, u8 rc);	/* for ISRs */
void enqueue_message_sw(u8 dst, u8 rc);	/* for other code */
  u8 dequeue_message(void);

extern struct message_queue mq;

#define __enqueue_message_hw(m)		\
do {					\
	struct mailbox *mb;		\
	mb = &mq.mbox_hw;		\
	mb->msg[mb->wri] = m;		\
	mb->wri++;			\
	mb->wri &= MBOX_MASK;		\
	mb->num++;			\
} while(0);

#define DST_SM_SHIFT		4

#define MAKE_MSG(dst_sm, ev_code)			\
	(((dst_sm) << DST_SM_SHIFT) | (ev_code))

#define MSG_DEST_SM(msg)				\
	((msg >> DST_SM_SHIFT) & 0x0F)

#define MSG_EV_CODE(msg)				\
	(msg & 0x0F)

#endif
