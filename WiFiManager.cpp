#include "WiFiManager.h"
#include <ArduinoJson.h>

WiFiManager::WiFiManager(WebServer& server) : server(server) {}

bool WiFiManager::loadCredentials(String &ssid, String &password) {
    File file = SPIFFS.open(wifi_credentials_file, FILE_READ);
    if (!file) {
        Serial.println("Failed to open file for reading.");
        return false;
    }
    JsonDocument json;
    DeserializationError error = deserializeJson(json, file);
    if (error) {
        Serial.print("Failed to read file, using default configuration. Error: ");
        Serial.println(error.f_str());
        file.close();
        return false;
    }
    ssid = json["ssid"].as<String>();
    password = json["password"].as<String>();
    file.close();
    Serial.println("Credentials loaded successfully.");
    return true;
}

void WiFiManager::saveCredentials(const char* ssid, const char* password) {
    JsonDocument json;
    json["ssid"] = ssid;
    json["password"] = password;
    File file = SPIFFS.open(wifi_credentials_file, FILE_WRITE);
    if (file) {
        serializeJson(json, file);
        file.close();
        Serial.println("Credentials saved successfully.");
    } else {
        Serial.println("Failed to open file for writing.");
    }
}

void WiFiManager::setupAccessPoint() {
    Serial.println("Setting up access point");
    WiFi.softAP("ESP32-AP", "12345678");
    Serial.println("Access Point started.");
    Serial.print("IP Address: ");
    Serial.println(WiFi.softAPIP());
}

bool WiFiManager::connectToWiFi() {
    String ssid, password;
    if (loadCredentials(ssid, password)) {
        Serial.print("Connecting to ");
        Serial.println(ssid);
        WiFi.begin(ssid.c_str(), password.c_str());
        int maxRetries = 20;
        while (WiFi.status() != WL_CONNECTED && maxRetries-- > 0) {
            delay(500);
            Serial.print(".");
        }
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\nConnected to WiFi");
            Serial.print("IP Address: ");
            Serial.println(WiFi.localIP());
            return true;
        } else {
            Serial.println("\nFailed to connect to WiFi");
            return false;
        }
    } else {
        Serial.println("No saved credentials");
        return false;
    }
}

void WiFiManager::setupServer() {
    server.on("/", std::bind(&WiFiManager::handleRoot, this));
    server.on("/save", HTTP_POST, std::bind(&WiFiManager::handleSave, this));
    server.on("/json", std::bind(&WiFiManager::handleJson, this));
    server.begin();
    Serial.println("Server started.");
}

void WiFiManager::handleRoot() {
    Serial.println("Handling root request");
    File file = SPIFFS.open("/index.html", FILE_READ);
    if (!file) {
        server.send(500, "text/plain", "File not found");
        Serial.println("Failed to open /index.html");
        return;
    }
    String html = file.readString();
    file.close();
    server.send(200, "text/html", html);
    Serial.println("Served /index.html");
}

void WiFiManager::handleSave() {
    Serial.println("Handling save request");
    if (server.hasArg("ssid") && server.hasArg("password")) {
        String ssid = server.arg("ssid");
        String password = server.arg("password");
        saveCredentials(ssid.c_str(), password.c_str());
        server.send(200, "text/plain", "Credentials saved, restarting...");
        Serial.println("Saved credentials, restarting...");
        delay(3000);
        ESP.restart();
    } else {
        server.send(400, "text/plain", "Invalid input");
        Serial.println("Invalid input");
    }
}

void WiFiManager::handleJson() {
    Serial.println("Handling JSON request");
    JsonDocument json;
    json["message"] = "Hello, this is a test JSON response!";
    json["status"] = "connected";
    String jsonResponse;
    serializeJson(json, jsonResponse);
    server.send(200, "application/json", jsonResponse);
    Serial.println("Served JSON response");
}
