// реализация на встроенном в ядро вебсервере

#pragma once
#include <Arduino.h>
#include <LittleFS.h>

#ifdef ESP8266
#include <ESP8266WebServer.h>
#else
#include <WebServer.h>
#endif

#include "SettingsBase.h"
#include "core/DnsWrapper.h"
#include "core/fs.h"
#include "core/ota.h"
#include "web/settings.h"

class SettingsESP : public sets::SettingsBase {
   public:
#ifndef SETT_NO_DB
    SettingsESP(const String& title = "", GyverDB* db = nullptr) : sets::SettingsBase(title, db), server(80) {}
#else
    SettingsESP(const String& title = "") : sets::SettingsBase(title), server(80) {}
#endif

    void begin(bool useDns = true) {
        if (useDns) _dns.begin();
        server.begin();

        server.on("/settings", HTTP_GET, [this]() {
            String auth = server.arg(F("auth"));
            String action = server.arg(F("action"));
            String id = server.arg(F("id"));
            String value = server.arg(F("value"));

            cors_h();
            parse(auth, action, id, value);
        });

        server.on("/fetch", HTTP_GET, [this]() {
            String auth = server.arg(F("auth"));
            cors_h();
            if (!authenticate(auth)) {
                server.send(401);
                return;
            }

            String path = server.arg(F("path"));
            File f = sets::FS.openRead(path);
            if (f) server.streamFile(f, "text/plain");
            else server.send(500);
        });

        server.on("/upload", HTTP_POST, [this]() {
            cors_h();
            server.send(200); }, [this]() {
            String auth = server.arg(F("auth"));
            if (!authenticate(auth)) return;

            HTTPUpload& upload = server.upload();
            if (upload.status == UPLOAD_FILE_START) {
                String path = server.arg(F("path"));
                _file = sets::FS.openWrite(path);
            } else if (upload.status == UPLOAD_FILE_WRITE) {
                if (_file) _file.write(upload.buf, upload.currentSize);
            } else if (upload.status == UPLOAD_FILE_END) {
                if (_file) _file.close();
            } });

        server.on("/ota", HTTP_POST, [this]() { 
            cors_h();
            server.send(Update.hasError() ? 500 : 200);
            if (!Update.hasError()) restart(); }, [this]() {
            String auth = server.arg(F("auth"));
            if (!authenticate(auth)) return;

            HTTPUpload& upload = server.upload();
            if (upload.status == UPLOAD_FILE_START) {
                sets::beginOta();
            } else if (upload.status == UPLOAD_FILE_WRITE) {
                Update.write(upload.buf, upload.currentSize);
            } else if (upload.status == UPLOAD_FILE_END) {
                Update.end(true);
            } });

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
        server.on("/favicon.svg", HTTP_GET, [this]() {
            gzip_h();
            cache_h();
            server.send_P(200, "image/svg+xml", (PGM_P)settings_favicon_gz, settings_favicon_gz_len);
        });
    }

    void tick() {
        _dns.tick();
        server.handleClient();
        sets::SettingsBase::tick();
    }

#ifdef ESP8266
    ESP8266WebServer server;
#else
    WebServer server;
#endif

   private:
    sets::DnsWrapper _dns;
    File _file;

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
#ifndef SETS_NO_CORS
        server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
        server.sendHeader(F("Access-Control-Allow-Private-Network"), F("true"));
        server.sendHeader(F("Access-Control-Allow-Methods"), F("*"));
#endif
    }
};