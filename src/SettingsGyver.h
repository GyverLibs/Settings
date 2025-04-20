// реализация на GyverHTTP

#pragma once
#include "SettingsT.h"

class SettingsGyver : public SettingsT<WiFiServer, WiFiClient> {
   public:
    using SettingsT<WiFiServer, WiFiClient>::SettingsT;

   private:
    String getMac() override {
        return WiFi.macAddress();
    }
    int getRSSI() override {
        return constrain(2 * (WiFi.RSSI() + 100), 0, 100);
    }
    IPAddress getIP() override {
        return WiFi.localIP();
    }
};