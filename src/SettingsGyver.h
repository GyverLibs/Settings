// реализация на GyverHTTP

#pragma once
#include <Arduino.h>
#include <GyverHTTP.h>
#include <LittleFS.h>

#include "SettingsBase.h"
#include "core/DnsWrapper.h"
#include "core/fs.h"
#include "core/ota.h"
#include "web/settings.h"

class SettingsGyver : public sets::SettingsBase {
   public:
#ifndef SETT_NO_DB
    SettingsGyver(const String& title = "", GyverDB* db = nullptr) : sets::SettingsBase(title, db), server(80) {}
#else
    SettingsGyver(const String& title = "") : sets::SettingsBase(title), server(80) {}
#endif

    void begin(bool useDns = true) {
        if (useDns) _dns.begin();
        server.begin();

#ifdef SETS_NO_CORS
        server.useCors(false);
#endif

        server.onRequest([this](ghttp::ServerBase::Request req) {
            switch (req.path().hash()) {
                case SH("/settings"):
                    parse(req.param("auth"), req.param("action"), req.param("id"), req.param("value").decodeUrl());
                    break;

                case SH("/fetch"):
                    if (authenticate(req.param("auth"))) {
                        File f = sets::FS.openRead(req.param("path").decodeUrl());
                        if (f) server.sendFile(f);
                        else server.send(500);
                    } else {
                        server.send(401);
                    }
                    break;

                case SH("/upload"):
                    if (authenticate(req.param("auth"))) {
                        File f = sets::FS.openWrite(req.param("path").decodeUrl());
                        if (f) {
                            req.body().writeTo(f);
                            server.send(200);
                        } else server.send(500);
                    } else {
                        server.send(401);
                    }
                    break;

                case SH("/ota"):
                    if (authenticate(req.param("auth"))) {
                        if (sets::beginOta() && req.body().writeTo(Update) && Update.end(true) && !Update.hasError()) {
                            server.send(200);
                            restart();
                        } else server.send(500);
                    } else {
                        server.send(401);
                    }
                    break;

                case SH("/script.js"):
                    server.sendFile_P(settings_script_gz, settings_script_gz_len, "text/javascript", true, true);
                    break;

                case SH("/style.css"):
                    server.sendFile_P(settings_style_gz, settings_style_gz_len, "text/css", true, true);
                    break;

                case SH("/favicon.svg"):
                    server.sendFile_P(settings_favicon_gz, settings_favicon_gz_len, "image/svg+xml", true, true);
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
        sets::SettingsBase::tick();
    }

    ghttp::Server<WiFiServer, WiFiClient> server;

   private:
    sets::DnsWrapper _dns;
    bool _rst = false;

    void send(uint8_t* data, size_t len) {
        server.sendFile(data, len);
    }
};