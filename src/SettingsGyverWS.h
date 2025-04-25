#pragma once

#include "./core/SyncWS.h"
#include "SettingsGyver.h"

class SettingsGyverWS : public SettingsGyver, private sets::SyncWS {
   public:
    using SettingsGyver::SettingsGyver;

    void begin(bool useDns = true) {
        setWSPort(81);
        SettingsGyver::begin(useDns);
        SyncWS::begin();
    }

    void stop() {
        SettingsGyver::stop();
        SyncWS::stop();
    }

    void tick() {
        SettingsGyver::tick();
        SyncWS::tick();
    }

    void onUpdate(UpdateCallback cb) = delete;
    using SettingsGyver::updater;

   private:
    void onData(uint8_t* data, size_t len) override {
        SettingsGyver::parseWS(data, len);
    }

    void sendWS(uint8_t* data, size_t len, bool broadcast) override {
        sets::SyncWS::send(data, len, broadcast);
    }
};