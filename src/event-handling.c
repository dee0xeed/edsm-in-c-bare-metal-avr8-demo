
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>

#include "itypes.h"
#include "init.h"
#include "usart.h"
#include "event-queue.h"
#include "event-handling.h"
#include "events.h"
#include "eeprom.h"
#include "config.h"

void on_swe_got_cmd(void)
{
	static char buf[32];
	static struct my_config c;

	 u8 *cmd = usart_rxbuf();
	int r;
	u16 addr;
	 u8 b;

	r = sscanf((char*)cmd, "c %hhd,%hhd,%hhd,%hhd", &c.a, &c.b, &c.c, &c.d);
	if (4 == r) {
		//strcpy(buf, "saving config...\r\n");
		eeprom_prep_save((u8*)&c, EEPROM_CONFIG_OFFSET, sizeof(struct my_config));
		//goto __send_reply;
		return;
	}

	r = sscanf((char*)cmd, "r %d", &addr);
	if (1 == r) {
		b = eeprom_get_byte(addr);
		sprintf(buf, "%.2X\r\n", b);
		goto __send_reply;
	}

	/* unsupported command */
	sprintf(buf, "got '%s'\r\n", (char*)cmd);

      __send_reply:
	usart_prep_tx((u8*)buf, strlen(buf));
}

void on_swe_eeprom_done(void)
{
	char *m = "config saved\r\n";
	usart_prep_tx((u8*)m, strlen(m));
}

void on_swe_reply_sent(void)
{
	/* now we are ready to accept next command */
	usart_prep_rx();
}

const event_handler event_handlers[] PROGMEM = {

	/* hardware events (interrupts) */
	               [0] = NULL,
	       [HWE_TIMER] = on_hwe_timer,
	   [HWE_USART_DRE] = on_hwe_usart_dre,
	[HWE_EEPROM_READY] = on_hwe_eeprom_ready,

	/* software events */
	     [SWE_GOT_CMD] = on_swe_got_cmd,
	  [SWE_REPLY_SENT] = on_swe_reply_sent,
	 [SWE_EEPROM_DONE] = on_swe_eeprom_done,
	             [256] = NULL
};

void handle_events(void)
{
	event_handler handler;
	u8 ev;

	for (;;) {

		cli();
		ev = get_event();
		sei();

		if (0 == ev)
			/* event queue is empty */
			break;

		handler = pgm_read_ptr(&event_handlers[ev]);

		if (NULL == handler)
			/* BUG */
			continue;

		handler();
	}
}
