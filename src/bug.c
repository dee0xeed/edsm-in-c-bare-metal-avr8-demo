
#include <avr/io.h>
#include <avr/interrupt.h>

void bug(void)
{
	/* disable interrupts */
	cli();
	/* turn LED on */
	PORTB |= _BV(PORTB5);
	/* and stop program */
	while (1);
}
