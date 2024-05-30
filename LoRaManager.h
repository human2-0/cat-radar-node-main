#ifndef LORAMANAGER_H
#define LORAMANAGER_H
#include <heltec.h>

class LoRaManager {
public:
    LoRaManager();
    bool setupLoRa(long frequency);
    void receiveLoRaMessage(const String &receiverID);
    
private:
    long _frequency;
};

#endif // LORAMANAGER_H
