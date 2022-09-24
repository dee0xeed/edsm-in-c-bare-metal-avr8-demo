
#ifndef EEPROM_H
#define EEPROM_H

#include "itypes.h"

  u8 eeprom_get_byte(u16 addr);
  u8 eeprom_prep_save(u8 *data, u16 addr, u16 len);
void eeprom_save_byte(void);

#endif
