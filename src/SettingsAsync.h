// реализация на встроенном в ядро вебсервере

#pragma once
#include <Arduino.h>
#include <LittleFS.h>

#ifdef ESP8266
#include <ESPAsyncTCP.h>
#else
#include <AsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

#include "SettingsBase.h"
#include "core/DnsWrapper.h"
#include "core/fs.h"
#include "core/ota.h"
#include "web/settings.h"

class SettingsAsync : public sets::SettingsBase {
   public:
#ifndef SETT_NO_DB
    SettingsAsync(const String &title = "", GyverDB *db = nullptr) : sets::SettingsBase(title, db), server(80) {}
#else
    SettingsAsync(const String &title = "") : sets::SettingsBase(title), server(80) {}
#endif

    void begin(bool useDns = true) {
        if (useDns) _dns.begin();
        server.begin();

        server.on("/settings", HTTP_GET, [this](AsyncWebServerRequest *request) {
            String auth, action, id, value;
            if (request->hasParam("auth")) auth = request->getParam("auth")->value();
            if (request->hasParam("action")) action = request->getParam("action")->value();
            if (request->hasParam("id")) id = request->getParam("id")->value();
            if (request->hasParam("value")) value = request->getParam("value")->value();

            _response = request->beginResponseStream("text/plain");
            cors_h(_response);
            parse(auth, action, id, value);
            request->send(_response);
            _response = nullptr;
        });

        server.on("/fetch", HTTP_GET, [this](AsyncWebServerRequest *request) {
            String auth, path;
            if (request->hasParam("auth")) auth = request->getParam("auth")->value();
            if (request->hasParam("path")) path = request->getParam("path")->value();

            if (authenticate(auth)) {
                AsyncWebServerResponse *response = request->beginResponse(ST_FS, path);
                cors_h(response);
                request->send(response);
            } else {
                sendCode(401, request);
            }
        });

        server.on("/upload", HTTP_POST, [this](AsyncWebServerRequest *request) { sendCode(200, request); }, [this](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
            String auth, path;
            if (request->hasParam("auth")) auth = request->getParam("auth")->value();
            if (request->hasParam("path")) path = request->getParam("path")->value();
            if (!authenticate(auth)) return;
            if (!index) _file = sets::FS.openWrite(path);
            if (len && _file) _file.write(data, len);
            if (final && _file) _file.close(); });

        server.on("/ota", HTTP_POST, [this](AsyncWebServerRequest *request) {
            sendCode(Update.hasError() ? 500 : 200, request);
            if (!Update.hasError()) restart(); }, [this](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
            String auth;
            if (request->hasParam("auth")) auth = request->getParam("auth")->value();
            if (!authenticate(auth)) return;
            if (!index) sets::beginOta(true, true);
            if (len) Update.write(data, len);
            if (final) Update.end(true); });

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
        server.on("/favicon.svg", HTTP_GET, [this](AsyncWebServerRequest *request) {
            AsyncWebServerResponse *response = request->beginResponse_P(200, "image/svg+xml", settings_favicon_gz, settings_favicon_gz_len);
            gzip_h(response);
            cache_h(response);
            request->send(response);
        });
    }

    void tick() {
        _dns.tick();
        sets::SettingsBase::tick();
    }

   private:
    AsyncWebServer server;
    AsyncResponseStream *_response = nullptr;
    sets::DnsWrapper _dns;
    File _file;

    void send(uint8_t *data, size_t len) {
        if (_response) _response->write(data, len);
    }
    void sendCode(int code, AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse(code);
        cors_h(response);
        request->send(response);
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
#ifndef SETS_NO_CORS
        response->addHeader(F("Access-Control-Allow-Origin"), F("*"));
        response->addHeader(F("Access-Control-Allow-Private-Network"), F("true"));
        response->addHeader(F("Access-Control-Allow-Methods"), F("*"));
#endif
    }
};