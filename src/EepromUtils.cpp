#include "EepromUtils.h"

DataInMemory data;
boolean isEepromError = false;

boolean isEepromValid(DataInMemory eeprom) 
{  
  return eeprom.magicNumber == MAGIC_NUMBER;
}

void initializeEEPROMData() {
  EEPROM.get(EEPROM_ADDR, data);
  Serial.println("EEPROM value: ");
  Serial.println(data.magicNumber);
  Serial.println(data.isPowerOn);
  Serial.println(data.time);
  if (!isEepromValid(data)) {
    data.magicNumber = MAGIC_NUMBER;
    data.isPowerOn = true;
    data.time = time(nullptr);
    EEPROM.put(EEPROM_ADDR, data);
    EEPROM.commit();  
    Serial.println("EEPROM initialized");
    EEPROM.get(EEPROM_ADDR, data);
    Serial.println("EEPROM value: ");
    Serial.println(data.magicNumber);
    Serial.println(data.isPowerOn);
    Serial.println(data.time);
  } else {
    Serial.println("EEPROM old value is valid");
  }
}

void initializeEEPROM() {
   // EEPROM (1)
  Serial.println("Init EEPROM:");
  EEPROM.begin(sizeof(DataInMemory));
}