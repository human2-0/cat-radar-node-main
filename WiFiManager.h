#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>

class WiFiManager {
public:
    WiFiManager(WebServer& server);
    void setupAccessPoint();
    bool connectToWiFi();
    void setupServer();
    void handleRoot();
    void handleSave();
    void handleJson();
    
private:
    const char* wifi_credentials_file = "/wifi_credentials.json";
    bool loadCredentials(String &ssid, String &password);
    void saveCredentials(const char* ssid, const char* password);
    WebServer& server;
};

#endif

