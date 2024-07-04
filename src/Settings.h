#pragma once
#include <Arduino.h>
#include <GyverHTTP.h>

#include "SettingsBase.h"
#include "web/settings.h"

#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif

class Settings : public SettingsBase {
   public:
    Settings(const String& title = "", GyverDB* db = nullptr) : SettingsBase(title, db), server(80) {}

    void begin() {
        server.begin();
        server.onRequest([this](ghttp::ServerBase::Request req) {
            switch (req.path().hash()) {
                case SH("/settings"):
                    parse(req.param("action"), req.param("id"), req.param("value").decodeUrl());
                    break;

                case SH("/script.js"):
                    server.sendFile_P(settings_script, sizeof(settings_script), "text/javascript", true, true);
                    break;

                case SH("/style.css"):
                    server.sendFile_P(settings_style, sizeof(settings_style), "text/css", true, true);
                    break;

                default:
                    server.sendFile_P(settings_index, sizeof(settings_index), "text/html", false, true);
                    break;
            }
        });
    }

    void tick() {
        server.tick();
        SettingsBase::tick();
    }

   private:
    ghttp::Server<WiFiServer, WiFiClient> server;

    void send(Text text) {
        server.send(text);
    }
};