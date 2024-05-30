#include <Arduino.h>
#include <WebServer.h>
#include "WiFiManager.h"
#include "LoRaManager.h"
#include <heltec.h>  

WebServer server(80);
WiFiManager wifiManager(server);  // Pass the server instance to WiFiManager
LoRaManager loraManager;

const long frequency = 433E6;  // Frequency for LoRa
const String receiverID = "RECEIVER_01"; // Unique ID for this device

bool loraInitialized = false;

void setup() {
    Serial.begin(115200);
    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS Mount Failed");
        return;
    }

     Heltec.begin(false /*DisplayEnable*/, true /*LoRa Enable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, frequency);

    if (!wifiManager.connectToWiFi()) {
        wifiManager.setupAccessPoint();
    } else {
        wifiManager.setupServer();
    }

    loraInitialized = loraManager.setupLoRa(frequency); // Initialize LoRa
}

void loop() {
    server.handleClient();
    if (loraInitialized) {
        loraManager.receiveLoRaMessage(receiverID); // Check for LoRa messages
    }
}
