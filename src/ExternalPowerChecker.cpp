#include "ExternalPowerChecker.h"

void readExternalPower() {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }

  EEPROM.get(EEPROM_ADDR, data);
  if (!isEepromValid(data)) {    
    Serial.println("EEPROM error!");
    isEepromError = true; 
    return;
  } else {
    isEepromError = false;
  }

  boolean isPowerNow = digitalRead(EXTERNAL_POWER_PROBE_PIN) == HIGH;
  boolean isPowerBefore = data.isPowerOn;

  #ifdef YESVITLO_DEBUG
    Serial.print("status: ");
    Serial.println(isPowerNow ? "on" : "off");
  #endif

  if (isPowerBefore != isPowerNow) {
    time_t now = time(nullptr);
    String durationText = getDuration(now, data.time);
    #ifdef YESVITLO_DEBUG
      Serial.println("status change detected, trying to send the message...");
      Serial.print("Duration: ");
      Serial.println(durationText);
    #endif
    if (isPowerNow) {
      sendMessageToAllSubscribedUsers(MSG_POWER_ON + durationText);
      data.isPowerOn = true;
    } else {
      sendMessageToAllSubscribedUsers(MSG_POWER_OFF + durationText);
      data.isPowerOn = false;
    }
    data.time = now;
    EEPROM.put(EEPROM_ADDR, data);
    EEPROM.commit(); 
    #ifdef YESVITLO_DEBUG
      Serial.println("The message sent");
    #endif
  }
}