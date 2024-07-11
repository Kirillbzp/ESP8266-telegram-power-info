#include "Constants.h"
#include "WifiManager.h"
#include "FileUtils.h"
#include "EepromUtils.h"
#include "MessageHandler.h"
#include "ExternalPowerChecker.h"

// Set settings in Constants.h

unsigned long bot_lasttime; // last time scan pin

void readExternalPower();
String getDuration(time_t now, time_t time);

void setup() {
  // General
  Serial.begin(115200);
  Serial.println();

  pinMode(EXTERNAL_POWER_PROBE_PIN, INPUT);
  configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
  setupTelegram();

  // EEPROM (1)
  initializeEEPROM();

  //File system
  initializeFileSystem();

  // Read Telegram data
  if (!readTelegramDataFromFile())
  {
    Serial.println("Can not create Telegram data file");
    return;
  }

  // Wifi:
  initializeWiFi();

  // RTC initialization for correct time
  Serial.print("Retrieving time: ");
  time_t now = time(nullptr);
  Serial.println(now);
  while (now < 24 * 3600) {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println();
  Serial.print("Time: ");
  Serial.println(now);

  // EEPROM (2)
  initializeEEPROMData();

  // Telegram bot
  const String commands = F("["
                            "{\"command\":\"showstatus\",  \"description\":\"Покаже поточний статус чи є світло\"},"
                            "{\"command\":\"start\", \"description\":\"Почати\"}"
                            "]");
  bot.setMyCommands(commands);
  //bot.waitForResponse = 25000;
}

void loop() {
  // if (WiFi.status() == WL_CONNECTED && !isEepromError) {
  //   ESP.wdtFeed(); // Watchdog reset for ESP8266
  // }
  if (millis() - bot_lasttime > BOT_MTBS)
  {
    updateNewMessages();
    readExternalPower();
    bot_lasttime = millis();
  }
}
