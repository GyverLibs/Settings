// реализация на GyverHTTP

#pragma once
#include <Arduino.h>
#include <GyverHTTP.h>

#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif

#ifndef SETT_NO_DNS
#include <DNSServer.h>
#endif

#include "SettingsBase.h"
#include "web/settings.h"

class SettingsGyver : public SettingsBase {
   public:
    SettingsGyver(const String& title = "", GyverDB* db = nullptr) : SettingsBase(title, db), server(80) {}

    void begin() {
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

#ifndef SETT_NO_DNS
        if (WiFi.getMode() == WIFI_AP || WiFi.getMode() == WIFI_AP_STA) {
            dns_f = 1;
            dns.start(53, "*", WiFi.softAPIP());
        }
#endif
    }

    void tick() {
#ifndef SETT_NO_DNS
        if (dns_f) dns.processNextRequest();
#endif
        server.tick();
        SettingsBase::tick();
    }

    ghttp::Server<WiFiServer, WiFiClient> server;

   private:
#ifndef SETT_NO_DNS
    DNSServer dns;
    bool dns_f = false;
#endif

    void send(uint8_t* data, size_t len) {
        server.sendFile(data, len);
    }
};