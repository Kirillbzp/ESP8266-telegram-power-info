#ifndef MESSAGE_H
#define MESSAGE

#include "MessageHandler.h"

WiFiClientSecure secured_client;
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
UniversalTelegramBot bot(BOTtoken, secured_client);

void setupTelegram()
{
    secured_client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
}

void updateNewMessages()
{
    #ifdef YESVITLO_DEBUG
      Serial.print("last message: ");
      Serial.println(bot.last_message_received);
    #endif
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    #ifdef YESVITLO_DEBUG
      Serial.print("new messages: ");
      Serial.println(numNewMessages);
    #endif
    while (numNewMessages)
    {
      #ifdef YESVITLO_DEBUG
        Serial.println("got response");
      #endif
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
}

void handleNewMessages(int numNewMessages)
{
  #ifdef YESVITLO_DEBUG
    Serial.print("handleNewMessages: ");
    Serial.println(numNewMessages);
  #endif

  for (int i = 0; i < numNewMessages; i++)
  {
    #ifdef YESVITLO_DEBUG
      Serial.print("Chat ID: ");
      Serial.println(bot.messages[i].chat_id);
      Serial.print("Chat Title: ");
      Serial.println(bot.messages[i].chat_title);
      Serial.print("Text: ");
      Serial.println(bot.messages[i].text);
    #endif

    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;
    String chat_title = bot.messages[i].chat_title;
    if (bot.messages[i].chat_title == "null")
    {
      chat_title = bot.messages[i].from_name;
    }
    if (chat_title == "")
      chat_title = "Guest";

    addSubscribedUser(chat_id, chat_title);

    if (text.startsWith("/start"))
    {
      showStartMessage(chat_id);
    }
    if (text.startsWith("/showstatus"))
    {
      showStatus(chat_id);      
    }
    if (text.startsWith("/clearallsubscribers") && chat_id == telegramData.adminId)
    {
      clearAllSubscribers(chat_id);
    }
    if (text.startsWith("/showalldata") && chat_id == telegramData.adminId)
    {
      showAllData(chat_id);
    }
    if (text.startsWith("/removesubscriber") && chat_id == telegramData.adminId)
    {
      removeSubscriber(chat_id, text);
    }
    if (text.startsWith("/replacesubscribers") && chat_id == telegramData.adminId)
    {
      replaceSubscribers(chat_id, text);
    }
    if (text.startsWith("/setthread") && chat_id == telegramData.adminId)
    {
      setThread(chat_id, text);
    }
  }
}

bool sendMessage(String chat_id, String message, String parse_mode)
{
  DynamicJsonDocument payload(MAX_MESSAGE_LENGTH);
  payload["chat_id"] = chat_id;
  payload["text"] = message;
  if (parse_mode != "")
    payload["parse_mode"] = parse_mode;

  for (auto& thread : telegramData.threads) {
    if (chat_id.equals(thread.first)) {
      payload["message_thread_id"] = thread.second;
      break;
    }
  }
  
  #ifdef YESVITLO_DEBUG
    serializeJson(payload, Serial);
  #endif
  if(!bot.sendPostMessage(payload.as<JsonObject>()))
  {
    #ifdef YESVITLO_DEBUG
      Serial.print("Error sending messge to: ");
      Serial.println(chat_id);
    #endif
    removeSubscribedUser(chat_id);
    #ifdef YESVITLO_DEBUG
      Serial.println("Removed from subscribers");
    #endif
  }
  return true;
}

void sendMessageToAllSubscribedUsers(String message)
{
  unsigned int users_processed = 0;

  for (auto& user : telegramData.users)
  {
    users_processed++;
    if (users_processed < MESSAGES_LIMIT_PER_SECOND)
    {
      sendMessage(user.first, message, "");
    }
    else
    {
      delay(BULK_MESSAGES_MTBS);
      users_processed = 0;
    }
  }
}

void showStartMessage(String chat_id)
{
  String welcome = "Привіт, я бот який підкаже чи є світло в Добрій Оселі.\n";
  welcome += "Можете надіслати мені одну з команд.\n";
  welcome += "/showstatus : Покаже поточний статус чи є світло\n";
  if(chat_id == telegramData.adminId)
  {
    welcome += "/showalldata : Показати всі збережені данні\n";
    welcome += "/clearallsubscribers : Видалити всіх підписників\n";
    welcome += "/removesubscriber {chat_id} : Видалити окремого підписника\n";
    welcome += "/replacesubscribers {Data_Content} : Замінити повністю данні в файлі "; welcome += TELEGRAM_DATA_FILENAME; welcome += "\n";
    welcome += "/setthread {chat_id} {thread_id} : Встановити thread_id для групи\n";
  }
  sendMessage(chat_id, welcome, "Markdown");
}

void showStatus(String chat_id)
{
  EEPROM.get(EEPROM_ADDR, data);
  if (!isEepromValid(data)) {    
    Serial.println("EEPROM error!");
    isEepromError = true; 
    return;
  } else {
    isEepromError = false;
  }
  
  time_t now = time(nullptr);
  String durationText = getDuration(now, data.time);

  boolean isPowerNow = digitalRead(EXTERNAL_POWER_PROBE_PIN) == HIGH;
  if (isPowerNow) {
    sendMessage(chat_id, MSG_POWER_ON_STATUS + durationText, "");
  } else {
    sendMessage(chat_id, MSG_POWER_OFF_STATUS + durationText, "");
  }
}

void clearAllSubscribers(String chat_id)
{
  telegramData.users.clear();

  if (saveTelegramDataToFile())
  {
    bot.sendMessage(chat_id, "All users removed", "");
  }
  else
  {
    bot.sendMessage(chat_id, "Something wrong, please try again (later?)", "");
  }
}

void showAllData(String chat_id)
{
  File telegramDataFile = SPIFFS.open(TELEGRAM_DATA_FILENAME, "r");
  // no file
  if (!telegramDataFile)
  {
    bot.sendMessage(chat_id, "No file", "");
    return;
  }
  size_t size = telegramDataFile.size();
  if (size > 2048)
  {
    bot.sendMessage(chat_id, "Data file is too large", "");
  }
  else
  {
    String file_content = telegramDataFile.readString();
    #ifdef YESVITLO_DEBUG
      Serial.println("Content: ");
      Serial.println(file_content);
    #endif
    bot.sendMessage(chat_id, file_content, "");
  }
}

void removeSubscriber(String chat_id, String text)
{
  String chat_id_to_remove = text.substring(String("/removesubscriber ").length());
  removeSubscribedUser(chat_id_to_remove);
  bot.sendMessage(chat_id, "Subscriber removed", "");
}

void replaceSubscribers(String chat_id, String text)
{
  String jsonData = text.substring(String("/replacesubscribers ").length());
  DynamicJsonDocument doc(2048);
  DeserializationError error = deserializeJson(doc, jsonData);
  if (error) {
    bot.sendMessage(chat_id, "Failed to parse data", "");
    return;
  }

  telegramData.adminId = doc["adminId"].as<String>();
  
  // Clear the vectors before populating with new data
  telegramData.users.clear();
  for (JsonObject user : doc["users"].as<JsonArray>()) {
    telegramData.users.push_back({user["chat_id"].as<String>(), user["name"].as<String>()});
  }

  telegramData.threads.clear();
  for (JsonObject thread : doc["threads"].as<JsonArray>()) {
    telegramData.threads.push_back({thread["chat_id"].as<String>(), thread["thread_id"].as<String>()});
  }

  if (saveTelegramDataToFile())
  {
    bot.sendMessage(chat_id, "All data replaced", "");
  }
  else
  {
    bot.sendMessage(chat_id, "Something wrong, please try again (later?)", "");
  }
}

void setThread(String chat_id, String text)
{
  String params = text.substring(String("/setThread ").length());
  int spaceIdx = params.indexOf(' ');
  String group_id = params.substring(0, spaceIdx);
  String thread_id = params.substring(spaceIdx + 1);
  addThreadForGroup(group_id, thread_id);
  bot.sendMessage(chat_id, "Done", "");
}

String getDuration(time_t now, time_t time)
{
  long timeDiff = now - time;
  int seconds = timeDiff % 60;
  int minutes = (timeDiff / 60) % 60;
  int hours = (timeDiff / 3600);

  return String(hours) + " год " + String(minutes) + " хв " + String(seconds) + " сек.";
}

#endif