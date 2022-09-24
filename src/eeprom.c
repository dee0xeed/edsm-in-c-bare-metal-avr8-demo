
#include <avr/io.h>
#include <avr/interrupt.h>

#include "eeprom.h"
#include "event-queue.h"
#include "events.h"
#include "bug.h"

u8 eeprom_get_byte(u16 addr)
{
	EEAR = addr & 0x03FF;
	EECR |=  1 << EERE;
	return EEDR;
}

static struct eep_save_job {
	 u8 *ptr;
	u16 adr;
	u16 cnt;
} job;

u8 eeprom_prep_save(u8 *data, u16 addr, u16 len)
{
	if (job.cnt)
		/* eeprom write is in progress already */
		return 1;

	job.ptr = data;
	job.adr = addr & 0x03FF;
	job.cnt = len;

	/* enable EEPROM ready interrupt */
	EECR |= _BV(EERIE);
	return 0;
}

void eeprom_save_byte(void)
{
	if (0 == job.cnt) {
		/* eeprom write is not in progress */
		bug();
		return;
	}

	EEAR = job.adr;
	EEDR = *job.ptr;

	cli();
	EECR |= _BV(EEMPE);
	EECR |= _BV(EEPE);
	sei();

	job.cnt--;
	if (0 == job.cnt) {
		cli();
		put_event(SWE_EEPROM_DONE, 1);
		sei();
		return;
	}

	/* advance RAM pointer and EEPROM address */
	job.ptr++;
	job.adr++;
	/* reenable EEPROM ready interrupt */
	EECR |= _BV(EERIE);
}
