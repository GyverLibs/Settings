#pragma once

#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif

#include <WebSocketsServer.h>

namespace sets {

class SyncWS {
   public:
    SyncWS() : _ws(81, "", "sets") {}

    ~SyncWS() {
        _clear();
    }

    void begin() {
        _ws.onEvent([this](uint8_t num, WStype_t type, uint8_t* data, size_t len) {
            switch (type) {
                case WStype_BIN:
                    _clear();
                    _buf = new uint8_t[len];
                    if (!_buf) return;

                    memcpy(_buf, data, len);
                    _len = len;
                    _id = num;
                    break;

                default: break;
            }
        });

        _ws.begin();
    }

    void stop() {
        _ws.close();
        _clear();
    }

    void tick() {
        _ws.loop();

        if (_buf) {
            onData(_buf, _len);
            _clear();
        }
    }

    void send(uint8_t* data, size_t len, bool broadcast) {
        if (broadcast) _ws.broadcastBIN(data, len);
        else _ws.sendBIN(_id, data, len);
    }

    virtual void onData(uint8_t* data, size_t len) = 0;

   private:
    WebSocketsServer _ws;
    uint8_t _id = 0;
    uint8_t* _buf = nullptr;
    size_t _len;

    void _clear() {
        if (_buf) delete[] _buf;
        _buf = nullptr;
    }
};

}  // namespace sets