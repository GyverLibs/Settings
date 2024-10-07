#pragma once
#include <Arduino.h>
#include <FS.h>
#include <StringUtils.h>

#ifndef SETT_NO_DB
#include <GyverDB.h>
#endif

#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif

#include "core/builder.h"
#include "core/colors.h"
#include "core/containers.h"
#include "core/fs.h"
#include "core/packet.h"
#include "core/timer.h"
#include "core/updater.h"

#define SETS_RESERVE 512

namespace sets {

class SettingsBase {
    typedef std::function<void(Builder& b)> BuildCallback;
    typedef std::function<void(Updater& upd)> UpdateCallback;

   public:
#ifndef SETT_NO_DB
    SettingsBase(const String& title = "", GyverDB* db = nullptr) : _title(title), _db(db) {
#else
    SettingsBase(const String& title = "") : _title(title) {
#endif
        useAutoUpdates(true);
    }

    // установить пароль на вебморду. Пустая строка "" чтобы отключить
    void setPass(Text pass) {
        _passh = pass.hash();
    }

    // установить заголовок страницы
    void setTitle(const String& title) {
        _title = title;
    }

    // установить период обновлений (умолч. 2500мс), 0 чтобы отключить
    void setUpdatePeriod(uint16_t prd) {
        _updPeriod = prd;
    }

// подключить базу данных
#ifndef SETT_NO_DB
    void attachDB(GyverDB* db) {
        _db = db;
    }
#endif

    // использовать автоматические обновления из БД (при изменении записи новое значение отправится в браузер) (умолч. true)
    void useAutoUpdates(bool use) {
        _dbupdates = use;
#ifndef SETT_NO_DB
        if (_db) _db->useUpdates(use);
#endif
    }

    // обработчик билда
    void onBuild(BuildCallback cb) {
        _build_cb = cb;
    }

    // обработчик обновлений
    void onUpdate(UpdateCallback cb) {
        _upd_cb = cb;
    }

    // тикер, вызывать в родительском классе
    void tick() {
#ifndef SETT_NO_DB
        if (_db) _db->tick();
#endif
        if (_rst) {
            delay(3000);
            ESP.restart();
        }
    }

    // перезагрузить страницу. Можно вызывать где угодно + в обработчике update
    void reload() {
        _reload = true;
    }

   protected:
    // отправка для родительского класса
    virtual void send(uint8_t* data, size_t len) {}

    bool authenticate(Text passh) {
        return !_passh || (_passh == passh.toInt32HEX());
    }

    void restart() {
        _rst = true;
    }

    // парсить запрос клиента
    void parse(Text passh, Text action, Text idtxt, Text value) {
        size_t id = idtxt.toInt32HEX();
        bool granted = authenticate(passh);

        switch (action.hash()) {
            case SH("discover"): {
                String str(F(R"raw({"type":"discover","name":")raw"));
                if (_title.length()) str += _title;
                else str += F("Unnamed");
                str += F(R"raw(","mac":")raw");
                str += WiFi.macAddress();
                str += "\"}";
                send((uint8_t*)str.c_str(), str.length());
            } break;

            case SH("load"):
                _sendBuild(granted);
                break;

            case SH("set"):
#ifndef SETT_NO_DB
                if (_db) {
                    if (_dbupdates) _db->useUpdates(false);
                    _db->update(id, value);
                    if (_dbupdates) _db->useUpdates(true);
                }
#endif
                if (_build_cb) {
                    Build action(Build::Type::Set, granted, id, value);
                    Builder b(this, action);
                    _build_cb(b);
                    if (b.isReload()) _sendBuild(granted);
                    else _answerEmpty();
                } else {
                    _answerEmpty();
                }
                break;

            case SH("click"):
                if (_build_cb) {
                    Build action(Build::Type::Click, granted, id);
                    Builder b(this, action);
                    _build_cb(b);
                    if (b.isReload()) _sendBuild(granted);
                    else _answerEmpty();
                } else {
                    _answerEmpty();
                }
                break;

            case SH("ping"):
#ifndef SETT_NO_DB
                if (_upd_cb || _db) {
#else
                if (_upd_cb) {
#endif
                    Packet p;
                    Updater upd(p);
                    p.beginObj();
                    p.addCode(Code::type, Code::update);
                    p.addUint(Code::rssi, constrain(2 * (WiFi.RSSI() + 100), 0, 100));
                    p.beginArr(Code::content);
#ifndef SETT_NO_DB
                    if (_db && _dbupdates) {
                        while (_db->updatesAvailable()) {
                            size_t id = _db->updateNext();
                            p.beginObj();
                            p.addUint(Code::id, id);
                            p.addKey(Code::value);
                            p.addFromDB(_db, id);
                            p.endObj();
                        }
                    }
#endif
                    if (_upd_cb) _upd_cb(upd);
                    p.endArr();
                    p.endObj();

                    if (_reload) {
                        _reload = false;
                        _sendBuild(granted);
                    } else {
                        send(p.buf(), p.length());
                    }
                } else {
                    _answerEmpty();
                }
                break;

            case SH("fs"):
                _sendFs(granted);
                break;

            case SH("remove"):
                if (granted) {
                    FS.remove(value);
                    _sendFs(true);
                }
                break;
        }
    }

   private:
    BuildCallback _build_cb = nullptr;
    UpdateCallback _upd_cb = nullptr;
    String _title;
    size_t _passh = 0;
#ifndef SETT_NO_DB
    GyverDB* _db = nullptr;
#endif
    uint16_t _updPeriod = 2500;
    bool _dbupdates = true;
    bool _rst = false;
    bool _reload = false;

    void _answerEmpty() {
        uint8_t p;
        send(&p, 0);
    }

    void _sendFs(bool granted) {
        String str;
        if (granted) FS.listDir(str, "/", true);

        Packet p;
        p.beginObj();
        p.addCode(Code::type, Code::fs);
        p.addText(Code::content, str);
        p.addUint(Code::used, FS.usedSpace());
        p.addUint(Code::total, FS.totalSpace());
        if (!granted) p.addText(Code::error, "Access denied");
        p.endObj();
        send(p.buf(), p.length());
    }

    void _sendBuild(bool granted) {
        if (_build_cb) {
            Packet p;
            p.reserve(SETS_RESERVE);
            p.beginObj();
            p.addCode(Code::type, Code::build);
            p.addUint(Code::ping, _updPeriod);
            p.addUint(Code::rssi, constrain(2 * (WiFi.RSSI() + 100), 0, 100));
            if (_title.length()) p.addText(Code::title, _title);
            if (_passh) p.addBool(Code::granted, granted);
#ifdef ATOMIC_FS_UPDATE
            p.addBool(Code::gzip, true);
#endif
            p.beginArr(Code::content);
            Build action(Build::Type::Build, granted);
#ifndef SETT_NO_DB
            Builder builder(this, action, &p, _db);
#else
            Builder builder(this, action, &p);
#endif
            _build_cb(builder);
            p.endArr();
            p.endObj();
            send(p.buf(), p.length());
        } else {
            _answerEmpty();
        }
    }
};

}  // namespace sets