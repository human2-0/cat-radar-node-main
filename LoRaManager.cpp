#include "LoRaManager.h"
#include <heltec.h>
#include <lora/LoRa.h>

LoRaManager::LoRaManager() {}

bool LoRaManager::setupLoRa(long frequency) {
    _frequency = frequency;

    Serial.begin(115200);


    Serial.print("Frequency: ");
    Serial.println(_frequency);

    if (!LoRa.begin(_frequency, true)) {
        Serial.println("Starting LoRa failed!");
        return false;
    }
    Serial.println("LoRa initialized successfully.");
    return true;
}

void LoRaManager::receiveLoRaMessage(const String &receiverID) {
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
        Serial.print("Received packet '");

        String incoming = "";
        while (LoRa.available()) {
            incoming += (char)LoRa.read();
        }

        Serial.print(incoming);

        int separatorIndex = incoming.indexOf(':');
        if (separatorIndex != -1) {
            String recipientID = incoming.substring(0, separatorIndex);
            String message = incoming.substring(separatorIndex + 1);

            if (recipientID == receiverID) {
                Serial.print("' intended for ");
                Serial.println(receiverID);
            } else {
                Serial.print("' not intended for ");
                Serial.println(receiverID);
            }
        } else {
            Serial.println("' invalid format");
        }

        Serial.print("' with RSSI ");
        Serial.println(LoRa.packetRssi());
    }
}
