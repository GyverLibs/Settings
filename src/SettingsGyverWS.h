#pragma once

#include "./core/SyncWS.h"
#include "SettingsGyver.h"

class SettingsGyverWS : public SettingsGyver, private sets::SyncWS {
   public:
    using SettingsGyver::SettingsGyver;

    // запустить. captive - запустить mdns для автооткрытия окна в режиме AP при подключении к точке
    // domain - домен, по которому есп будет доступна в локальной сети по адресу домен.local
    void begin(bool captive = true, const char* domain = nullptr) {
        setWSPort(81);
        SettingsGyver::begin(captive, domain);
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