// реализация на встроенном в ядро вебсервере

#pragma once
#include <Arduino.h>
#include <LittleFS.h>

#ifdef ESP8266
#include <ESP8266WebServer.h>
#else
#include <WebServer.h>
#endif

#include "./core/DnsWrapper.h"
#include "./core/SettingsBase.h"
#include "./core/ota.h"
#include "./web/settings.h"

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
            _first = true;
            String auth = server.arg(F("auth"));
            String action = server.arg(F("action"));
            String id = server.arg(F("id"));
            String value = server.arg(F("value"));

            cors_h();
            parse(Text(auth).toInt32HEX(),
                  Text(action).hash(),
                  Text(id).toInt32HEX(),
                  value);
        });

        server.on("/fetch", HTTP_GET, [this]() {
            String auth = server.arg(F("auth"));
            cors_h();
            if (!authenticate(Text(auth).toInt32HEX())) {
                server.send(401);
                return;
            }

            String path = server.arg(F("path"));
            File f = fs.openRead(path.c_str());
            if (f) server.streamFile(f, "text/plain");
            else server.send(500);
            if (fetch_cb) fetch_cb(path);
        });

        server.on("/upload", HTTP_POST, [this]() {
            cors_h();
            server.send(200); }, [this]() {
            String auth = server.arg(F("auth"));
            if (!authenticate(Text(auth).toInt32HEX())) return;

            HTTPUpload& upload = server.upload();
            if (upload.status == UPLOAD_FILE_START) {
                String path = server.arg(F("path"));
                _file = fs.openWrite(path.c_str());
            } else if (upload.status == UPLOAD_FILE_WRITE) {
                if (_file) _file.write(upload.buf, upload.currentSize);
            } else if (upload.status == UPLOAD_FILE_END) {
                if (_file) _file.close();
                if (upload_cb) {
                    String path = server.arg(F("path"));
                    upload_cb(path);
                }
            } });

        server.on("/ota", HTTP_POST, [this]() { 
            cors_h();
            server.send(Update.hasError() ? 500 : 200);
            if (!Update.hasError()) restart(); }, [this]() {
            String auth = server.arg(F("auth"));
            if (!authenticate(Text(auth).toInt32HEX())) return;

            HTTPUpload& upload = server.upload();
            if (upload.status == UPLOAD_FILE_START) {
                sets::beginOta();
            } else if (upload.status == UPLOAD_FILE_WRITE) {
                Update.write(upload.buf, upload.currentSize);
            } else if (upload.status == UPLOAD_FILE_END) {
                Update.end(true);
            } });

        server.onNotFound([this]() {
            index_h();
        });
        server.on("/", HTTP_GET, [this]() {
            index_h();
        });
        server.on("/script.js", HTTP_GET, [this]() {
            gzip_h();
            cache_h();
            server.send_P(200, "text/javascript", (PGM_P)settings_script_gz, sizeof(settings_script_gz));
        });
        server.on("/style.css", HTTP_GET, [this]() {
            gzip_h();
            cache_h();
            server.send_P(200, "text/css", (PGM_P)settings_style_gz, sizeof(settings_style_gz));
        });
        server.on("/favicon.svg", HTTP_GET, [this]() {
            gzip_h();
            cache_h();
            server.send_P(200, "image/svg+xml", (PGM_P)settings_favicon_gz, sizeof(settings_favicon_gz));
        });
        server.on("/custom.js", HTTP_GET, [this]() {
            if (!custom.p) server.send(500);
            else {
                if (custom.gz) gzip_h();
                if (!custom.isFile) server.send_P(200, "text/javascript", custom.p, custom.len);
                else {
                    File f = fs.openRead(custom.p);
                    if (f) server.streamFile(f, "text/javascript");
                    else server.send(500);
                }
            }
        });
    }

    void stop() {
        server.stop();
        _dns.stop();
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
    bool _first = true;

    String getMac() override {
        return WiFi.macAddress();
    }
    int getRSSI() override {
        return constrain(2 * (WiFi.RSSI() + 100), 0, 100);
    }
    IPAddress getIP() override {
        return WiFi.localIP();
    }

    void answer(uint8_t* data, size_t len) override {
        if (_first) {
            _first = false;
            server.setContentLength(CONTENT_LENGTH_UNKNOWN);
            server.send(200, "text/plain");
        }
        server.sendContent((const char*)data, len);
    }

    void index_h() {
        gzip_h();
        no_cache_h();
        server.send_P(200, "text/html", (PGM_P)settings_index_gz, sizeof(settings_index_gz));
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