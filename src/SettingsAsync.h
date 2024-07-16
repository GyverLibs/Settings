// реализация на встроенном в ядро вебсервере

#pragma once
#include <Arduino.h>

#ifdef ESP32
#include <AsyncTCP.h>
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

#include "SettingsBase.h"
#include "web/settings.h"

class SettingsAsync : public SettingsBase {
   public:
    SettingsAsync(const String &title = "", GyverDB *db = nullptr) : SettingsBase(title, db), server(80) {}

    void begin() {
        server.begin();

        server.on("/settings", HTTP_GET, [this](AsyncWebServerRequest *request) {
            String action, id, value;
            if (request->hasParam("action")) action = request->getParam("action")->value();
            if (request->hasParam("id")) id = request->getParam("id")->value();
            if (request->hasParam("value")) value = request->getParam("value")->value();

            _response = request->beginResponseStream("text/plain");
            cors_h(_response);
            parse(action, id, value);
            request->send(_response);
            _response = nullptr;
        });

        server.onNotFound([this](AsyncWebServerRequest *request) {
            AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", settings_index_gz, settings_index_gz_len);
            gzip_h(response);
            no_cache_h(response);
            request->send(response);
        });
        server.on("/script.js", HTTP_GET, [this](AsyncWebServerRequest *request) {
            AsyncWebServerResponse *response = request->beginResponse_P(200, "text/javascript", settings_script_gz, settings_script_gz_len);
            gzip_h(response);
            cache_h(response);
            request->send(response);
        });
        server.on("/style.css", HTTP_GET, [this](AsyncWebServerRequest *request) {
            AsyncWebServerResponse *response = request->beginResponse_P(200, "text/css", settings_style_gz, settings_style_gz_len);
            gzip_h(response);
            cache_h(response);
            request->send(response);
        });
    }

    void tick() {
        SettingsBase::tick();
    }

   private:
    AsyncWebServer server;
    AsyncResponseStream *_response = nullptr;

    void send(uint8_t *data, size_t len) {
        if (_response) _response->write(data, len);
    }

    void gzip_h(AsyncWebServerResponse *response) {
        response->addHeader(F("Content-Encoding"), F("gzip"));
    }
    void cache_h(AsyncWebServerResponse *response) {
        response->addHeader(F("Cache-Control"), F("max-age=604800"));
    }
    void no_cache_h(AsyncWebServerResponse *response) {
        response->addHeader(F("Cache-Control"), F("no-cache, no-store, must-revalidate"));
        response->addHeader(F("Pragma"), F("no-cache"));
        response->addHeader(F("Expires"), F("0"));
    }
    void cors_h(AsyncWebServerResponse *response) {
        response->addHeader(F("Access-Control-Allow-Origin"), F("*"));
        response->addHeader(F("Access-Control-Allow-Private-Network"), F("true"));
        response->addHeader(F("Access-Control-Allow-Methods"), F("*"));
    }
};