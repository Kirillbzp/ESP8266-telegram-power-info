#include "FileUtils.h"

TelegramData telegramData;

void initializeFileSystem() {
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system, formatting...");
    if (SPIFFS.format()) {
        Serial.println("File system formatted successfully. Trying to mount again...");
        if(SPIFFS.begin()) {
            Serial.println("File system mounted successfully after formatting.");
        } else {
            Serial.println("Failed to mount file system after formatting.");
            return;
        }
    } else {
        Serial.println("Failed to format the file system.");
        return;
    }
  } else {
      Serial.println("File system mounted successfully on first attempt.");
  }
}

bool createTelegramDataFile()
{
  telegramData.adminId = USER_ADMIN_ID;
  telegramData.users.clear();
  telegramData.threads.clear();

  if (!saveTelegramDataToFile())
  {
    return false;
  }

  return true;
}

bool readTelegramDataFromFile() {
  File file = SPIFFS.open(TELEGRAM_DATA_FILENAME, "r");
  if (!file) {
    Serial.println("Failed to open users file. Create new");
    if (!createTelegramDataFile())
    {
      return false;
    }
  }

  size_t size = file.size();
  std::unique_ptr<char[]> buf(new char[size]);

  file.readBytes(buf.get(), size);
  file.close();

  DynamicJsonDocument doc(2048);
  DeserializationError error = deserializeJson(doc, buf.get());
  if (error) {
    Serial.println("Failed to parse users file");
    return false;
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

  return true;
}

bool saveTelegramDataToFile() {
  DynamicJsonDocument doc(1024);

  doc["adminId"] = telegramData.adminId;
  
  JsonArray users = doc.createNestedArray("users");
  for (auto& user : telegramData.users) {
    JsonObject userObj = users.createNestedObject();
    userObj["chat_id"] = user.first;
    userObj["name"] = user.second;
  }

  JsonArray threads = doc.createNestedArray("threads");
  for (auto& thread : telegramData.threads) {
    JsonObject threadObj = threads.createNestedObject();
    threadObj["chat_id"] = thread.first;
    threadObj["thread_id"] = thread.second;
  }

  File file = SPIFFS.open(TELEGRAM_DATA_FILENAME, "w");
  if (!file) {
    Serial.println("Failed to open users file for writing");
    return false;
  }

  serializeJson(doc, file);
  file.close();
  return true;
}

void addSubscribedUser(String chat_id, String user_name) {
  #ifdef YESVITLO_DEBUG
    Serial.println("User to add: ");
    Serial.print("ID: ");
    Serial.println(chat_id);
    Serial.print("Name: ");
    Serial.println(user_name);
  #endif

  for (auto& user : telegramData.users) {
    if (user.first == chat_id) {
      #ifdef YESVITLO_DEBUG
        Serial.println("User is in the list");
      #endif
      // If user is already in the list, update the name. 
      // But if the name is Guest, do not update it
      if ( user.second != user_name && user_name != "Guest")
      {
        user.second = user_name;
        saveTelegramDataToFile();
      }
      return;
    }
  }
  
  // Add new user
  telegramData.users.push_back({chat_id, user_name});
  saveTelegramDataToFile();
}

void removeSubscribedUser(String chat_id) {
  for (auto it = telegramData.users.begin(); it != telegramData.users.end(); ++it) {
    if (it->first == chat_id) {
      telegramData.users.erase(it);
      saveTelegramDataToFile();
      return;
    }
  }
  Serial.println("User not found");
}

void addThreadForGroup(String chat_id, String thread_id) {
  #ifdef YESVITLO_DEBUG
    Serial.print("Adding thread ");
    Serial.print(thread_id);
    Serial.print(" for group ");
    Serial.println(chat_id);

  #endif
  for (auto& thread : telegramData.threads) {
    if (thread.first == chat_id) {
      if(thread.second != thread_id)
      {
        thread.second = thread_id;
        saveTelegramDataToFile();
      }
      return;
    }
  }
  
  telegramData.threads.push_back({chat_id, thread_id});
  saveTelegramDataToFile();
}

void removeThreadForGroup(String chat_id) {
  for (auto it = telegramData.threads.begin(); it != telegramData.threads.end(); ++it) {
    if (it->first == chat_id) {
      telegramData.threads.erase(it);
      saveTelegramDataToFile();
      return;
    }
  }
  Serial.println("Thread not found");
}

