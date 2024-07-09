#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <ArduinoJson.h>
#include <FS.h>

#include "Constants.h"

struct TelegramData {
  String adminId;
  std::vector<std::pair<String, String>> users;
  std::vector<std::pair<String, String>> threads;
};

extern TelegramData telegramData;

void initializeFileSystem();
bool createTelegramDataFile();
bool readTelegramDataFromFile();
bool saveTelegramDataToFile();

void addSubscribedUser(String chat_id, String user_name);
void removeSubscribedUser(String chat_id);
void addThreadForGroup(String chat_id, String thread_id);
void removeThreadForGroup(String chat_id);

#endif