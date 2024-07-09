#include "WifiManager.h"

void initializeWiFi() {
  Serial.print("Connecting to Wifi SSID: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());
}