
#include <avr/io.h>
#include <avr/interrupt.h>

#include "message-queue.h"
#include "itypes.h"
#include "edsm.h"

ISR(TIMER0_COMPA_vect)
{
	__enqueue_message_hw(MAKE_MSG(M0_LED, 0));
}

ISR(USART_UDRE_vect)
{
	/* disable USART DRE interrupt */
	UCSR0B &= ~_BV(UDRIE0);
	enqueue_message_hw(M1_USART, R0_);
}

ISR(USART_RX_vect)
{
	/* disable USART RXC interrupt (for the sake of uniformity) */
	UCSR0B &= ~_BV(RXCIE0);
	enqueue_message_hw(M1_USART, R0_);
}

//ISR(EE_READY_vect)
//{
//	/* disable EEPROM ready interrupt */
//	EECR &= ~_BV(EERIE);
//	enqueue_message_hw(MAKE_MSG(EEPROM_SM, 0));
//}
