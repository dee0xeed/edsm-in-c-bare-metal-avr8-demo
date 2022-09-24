
#include <stdlib.h>
#include <avr/interrupt.h>
#include "message-queue.h"

struct message_queue mq; // = {0};

void init_message_queue(void)
{
	struct mailbox *mb;

	mb = &mq.mbox_hw;
	mb->num = 0;
	mb->wri = 0;
	mb->rdi = 0;

	mb = &mq.mbox_sw;
	mb->num = 0;
	mb->wri = 0;
	mb->rdi = 0;
}

static void do_enqueue_message(u8 msg, struct mailbox *mb)
{
	mb->msg[mb->wri] = msg;
	mb->wri ++;
	mb->wri &= MBOX_MASK;
	mb->num ++;
}

void enqueue_message_hw(u8 dst, u8 rc)
{
	u8 msg = MAKE_MSG(dst,rc);
	do_enqueue_message(msg, &mq.mbox_hw);
}

void enqueue_message_sw(u8 dst, u8 rc)
{
	u8 msg = MAKE_MSG(dst,rc);
	do_enqueue_message(msg, &mq.mbox_sw);
}

static u8 do_dequeue_message(struct mailbox *mb)
{
	u8 msg = 0xFF;

	if (0 == mb->num)
		goto __end;

	msg = mb->msg[mb->rdi];
	mb->rdi ++;
	mb->rdi &= MBOX_MASK;
	mb->num --;

      __end:
	return msg;
}

u8 dequeue_message(void)
{
	u8 msg;

	/* first deal with interrupts */
	cli();
	msg = do_dequeue_message(&mq.mbox_hw);
	sei();

	/* then with software generated messages */
	if (0xFF == msg)
		msg = do_dequeue_message(&mq.mbox_sw);

	return msg;
}
