#pragma once

#include "./core/SyncWS.h"
#include "SettingsESP.h"

class SettingsESPWS : public SettingsESP, private sets::SyncWS {
   public:
    using SettingsESP::SettingsESP;

    void begin(bool useDns = true) {
        setWSPort(81);
        SettingsESP::begin(useDns);
        SyncWS::begin();
    }

    void stop() {
        SettingsESP::stop();
        SyncWS::stop();
    }

    void tick() {
        SettingsESP::tick();
        SyncWS::tick();
    }
    
    void onUpdate(UpdateCallback cb) = delete;
    using SettingsESP::updater;

   private:
    void onData(uint8_t* data, size_t len) override {
        SettingsESP::parseWS(data, len);
    }

    void sendWS(uint8_t* data, size_t len, bool broadcast) override {
        sets::SyncWS::send(data, len, broadcast);
    }
};