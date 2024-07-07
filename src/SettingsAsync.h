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

            _request = request;
            parse(action, id, value);
            _request = nullptr;
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
    AsyncWebServerRequest *_request = nullptr;

    void send(Text text) {
        if (!_request) return;
        AsyncResponseStream *response = _request->beginResponseStream("application/octet-stream");
        response->write(text.str(), text.length());
        _request->send(response);
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
};