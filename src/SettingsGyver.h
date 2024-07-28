// реализация на GyverHTTP

#pragma once
#include <Arduino.h>
#include <GyverHTTP.h>

#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif

#include "SettingsBase.h"
#include "core/ESP_DNS.h"
#include "web/settings.h"

class SettingsGyver : public SettingsBase {
   public:
    SettingsGyver(const String& title = "", GyverDB* db = nullptr) : SettingsBase(title, db), server(80) {}

    void begin() {
        _dns.begin();
        server.begin();

        server.onRequest([this](ghttp::ServerBase::Request req) {
            switch (req.path().hash()) {
                case SH("/settings"):
                    parse(req.param("action"), req.param("id"), req.param("value").decodeUrl());
                    break;

                case SH("/script.js"):
                    server.sendFile_P(settings_script_gz, settings_script_gz_len, "text/javascript", true, true);
                    break;

                case SH("/style.css"):
                    server.sendFile_P(settings_style_gz, settings_style_gz_len, "text/css", true, true);
                    break;

                default:
                    server.sendFile_P(settings_index_gz, settings_index_gz_len, "text/html", false, true);
                    break;
            }
        });
    }

    void tick() {
        _dns.tick();
        server.tick();
        SettingsBase::tick();
    }

    ghttp::Server<WiFiServer, WiFiClient> server;

   private:
    sets::ESP_DNS _dns;
    void send(uint8_t* data, size_t len) {
        server.sendFile(data, len);
    }
};