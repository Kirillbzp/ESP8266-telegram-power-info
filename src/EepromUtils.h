#ifndef EEPROMUTILS_H
#define EEPROMUTILS_H

#include <Arduino.h>
#include <EEPROM.h>

#include "Constants.h"

struct DataInMemory {
  uint32_t magicNumber; // To check is data valid
  bool isPowerOn = true;
  time_t time = 0;
};

extern DataInMemory data;
extern boolean isEepromError;

boolean isEepromValid(DataInMemory eeprom);
void initializeEEPROMData();
void initializeEEPROM();

#endif