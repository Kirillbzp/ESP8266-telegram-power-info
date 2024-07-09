#ifndef MESSAGE_H
#define MESSAGE

#include <Arduino.h>
#include <UniversalTelegramBot.h>  
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>

#include "Constants.h"
#include "FileUtils.h"
#include "EepromUtils.h"

extern UniversalTelegramBot bot;

void setupTelegram();

void updateNewMessages();
void handleNewMessages(int numNewMessages);

void showStartMessage(String chat_id);
void showStatus(String chat_id);
void showAllData(String chat_id);
void clearAllSubscribers(String chat_id);
void removeSubscriber(String chat_id, String text);
void replaceSubscribers(String chat_id, String text);
void setThread(String chat_id, String text);

bool sendMessage(String chat_id, String message);
void sendMessageToAllSubscribedUsers(String message);

String getDuration(time_t now, time_t time);

#endif