
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "message-queue.h"
#include "edsm.h"

int main(void)
{
	cli();
	init_message_queue();
	start_state_machines();
//	init_software();
//	init_hardware();
//	usart_prep_rx();
	sei();
    sleep_enable();

	for (;;) {
		process_messages();
		//sleep_mode();
		sleep_cpu();
	}
}
