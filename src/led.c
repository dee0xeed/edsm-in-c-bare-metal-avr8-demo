
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "bug.h"
#include "init.h"
#include "led.h"
#include "itypes.h"
#include "message-queue.h"
#include "edsm.h"

void led_init_enter(struct edsm *me);

void led_work_R0(struct edsm *me);
void led_work_R1(struct edsm *me);
void led_work_R2(struct edsm *me);

struct reflex led_init_reflexes[] = {

	[0] = {
		.action = NULL,
		.next_state = S1_LED_WORK,
	},
};

struct reflex led_work_reflexes[] = {

	[0] = {	/* timer interrupt */
		.action = led_work_R0,
		.next_state = 0xFF,
	},

	[1] = {
		.action = led_work_R1,
		.next_state = 0xFF,

	},

	[2] = {
		.action = led_work_R2,
		.next_state = 0xFF,
	},
};

struct state led_states[] = {

	[S0_LED_INIT] = {
		.name = "init",
		.enter = led_init_enter,
		.reflexes = led_init_reflexes,
		.n_reflexes = ARRAY_SIZE(led_init_reflexes)
	},

	[S1_LED_WORK] = {
		.name = "work",
		.enter = NULL,
		.reflexes = led_work_reflexes,
		.n_reflexes = ARRAY_SIZE(led_work_reflexes)
	},
};

static void _init_timer(void)
{
	TCCR0B = 0x00;		/* stop timer0 */
	TCCR0A = 0x02;		/* CTC mode */
	 TCNT0 = 0x00;		/* clear counter0 */
	TIMSK0 = _BV(OCIE0A);	/* enable intr. on "output compare match" event */

	 OCR0A = TOP(TICKRATE, PRESCALE); /* set TOP value for the counter */

	switch (PRESCALE) {

	case 1024:
		/* start timer0, /1024 prescale */
		TCCR0B = 0x05;
	break;

	case 256:
		/* start timer0, /256 prescale */
		TCCR0B = 0x04;
	break;

	case 64:
		/* start timer0, /64 prescale */
		TCCR0B = 0x03;
	break;

	default:
		bug();
	}
}

void led_init_enter(struct edsm *me)
{
	 DDRB |=  _BV(DDB5);	/* output   */
	PORTB &= ~_BV(PORTB5);	/* turn LED off */

	_init_timer();

	enqueue_message_sw(M0_LED, 0);
}

void led_work_R0(struct edsm *me)
{
	static u16 cnt = 0;

	cnt++;

	if (TICKRATE == cnt) {
		/* turn LED on */
		PORTB |= _BV(PORTB5);
		cnt = 0;
	}

	if (TICKRATE >> 4 == cnt) {
		/* turn LED off */
		PORTB &= ~_BV(PORTB5);
	}
}

void led_work_R1(struct edsm *me)
{
}

void led_work_R2(struct edsm *me)
{
}
