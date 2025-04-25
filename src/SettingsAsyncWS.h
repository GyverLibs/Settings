#pragma once

#include "SettingsAsync.h"

class SettingsAsyncWS : public SettingsAsync {
   public:
    using SettingsAsync::SettingsAsync;

    void begin(bool useDns = true) {
        setWSPort(80);
        SettingsAsync::begin(useDns);
        ws.onEvent([this](AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len) {
            switch (type) {
                case WS_EVT_DATA:
                    _id = client->id();
                    SettingsAsync::parseWS(data, len);
                    break;
                default:
                    break;
            }
        });
        server.addHandler(&ws);
    }

    void stop() {
        SettingsAsync::stop();
        ws.closeAll();
    }

    void tick() {
        SettingsAsync::tick();
        ws.cleanupClients();
    }

    void onUpdate(UpdateCallback cb) = delete;
    using SettingsAsync::updater;

   private:
    AsyncWebSocket ws{"/"};
    uint32_t _id;

    void sendWS(uint8_t* data, size_t len, bool broadcast) override {
        broadcast ? ws.binaryAll(data, len) : ws.binary(_id, data, len);
    }
};