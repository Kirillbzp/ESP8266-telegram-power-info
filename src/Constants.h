#ifndef CONSTANTS_H
#define CONSTANTS_H

// DEBUG settings
//#define TELEGRAM_DEBUG 1 // uncomment to receive debug messages from Telegram library
#define YESVITLO_DEBUG 1 // uncomment to receive debug messages from bot

// EEPROM settings
#define EEPROM_ADDR 0 
#define MAGIC_NUMBER 0xCAFEBABE

// Telegram settings
#define MAX_MESSAGE_LENGTH 1500
#define USER_ADMIN_ID "339261838" // For now there are no methods to receive the bot owner ID. So put your ID here
//#define BOTtoken "7209344650:AAH5H8cqOlnoJ8CsNLkIOzBXpBZy57kvaJM" // змініть на ваше
#define BOTtoken "6708770852:AAGDQZJsTQkkmo0fMcnDq1xREpwUeh6g5cg" // use yours token received from the Bot_father

// WiFi settings
#define WIFI_SSID "Karapuzzzz_IoT" // use yours WiFi SSID (name)
#define WIFI_PASSWORD "13266132" // use yours WiFi password

// Text messages
#define MSG_POWER_ON "🟢 З'явилося світло 💡\nЧас відсутності: "
#define MSG_POWER_OFF "🔴 Світло зникло 🔦\nЧас присутності: "
#define MSG_POWER_ON_STATUS "🟢 Зараз світло є 💡\nвже: "
#define MSG_POWER_OFF_STATUS "🔴 Зараз світла немає 🔦\nвже: "

// Power probe pin
#define EXTERNAL_POWER_PROBE_PIN 4

#define SUBSCRIBED_USERS_FILENAME "/users.json"       // OLD Filename for local storage
#define TELEGRAM_DATA_FILENAME "/telegram_data.json"  // Filename for local storage
#define BOT_MTBS 5000                         // time between scan pin
#define BULK_MESSAGES_MTBS 1500               // time between send messages, 1.5 seconds
#define MESSAGES_LIMIT_PER_SECOND 25           // Telegram API have limit for bulk messages ~30 messages per second


#endif // CONSTANTS_H