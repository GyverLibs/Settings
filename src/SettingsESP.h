// реализация на встроенном в ядро вебсервере

#pragma once
#include <Arduino.h>

#ifdef ESP8266
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#else
#include <WebServer.h>
#include <WiFi.h>
#endif

#ifndef SETT_NO_DNS
#include <DNSServer.h>
#endif

#include "SettingsBase.h"
#include "web/settings.h"

class SettingsESP : public SettingsBase {
   public:
    SettingsESP(const String& title = "", GyverDB* db = nullptr) : SettingsBase(title, db), server(80) {}

    void begin() {
        server.begin();

        server.on("/settings", HTTP_GET, [this]() {
            String action = server.arg(F("action"));
            String id = server.arg(F("id"));
            String value = server.arg(F("value"));

            cors_h();
            parse(action, id, value);
        });

        server.onNotFound([this]() {
            gzip_h();
            no_cache_h();
            server.send_P(200, "text/html", (PGM_P)settings_index_gz, settings_index_gz_len);
        });
        server.on("/script.js", HTTP_GET, [this]() {
            gzip_h();
            cache_h();
            server.send_P(200, "text/javascript", (PGM_P)settings_script_gz, settings_script_gz_len);
        });
        server.on("/style.css", HTTP_GET, [this]() {
            gzip_h();
            cache_h();
            server.send_P(200, "text/css", (PGM_P)settings_style_gz, settings_style_gz_len);
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
        server.handleClient();
        SettingsBase::tick();
    }

#ifdef ESP8266
    ESP8266WebServer server;
#else
    WebServer server;
#endif

   private:
#ifndef SETT_NO_DNS
    DNSServer dns;
    bool dns_f = false;
#endif

    void send(uint8_t* data, size_t len) {
        server.setContentLength(len);
        server.send(200, "text/plain");
        server.sendContent((const char*)data, len);
    }

    void gzip_h() {
        server.sendHeader(F("Content-Encoding"), F("gzip"));
    }
    void cache_h() {
        server.sendHeader(F("Cache-Control"), F("max-age=604800"));
    }
    void no_cache_h() {
        server.sendHeader(F("Cache-Control"), F("no-cache, no-store, must-revalidate"));
        server.sendHeader(F("Pragma"), F("no-cache"));
        server.sendHeader(F("Expires"), F("0"));
    }
    void cors_h() {
        server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
        server.sendHeader(F("Access-Control-Allow-Private-Network"), F("true"));
        server.sendHeader(F("Access-Control-Allow-Methods"), F("*"));
    }
};