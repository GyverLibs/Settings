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
#include "core/logger.h"
#include "core/packet.h"
#include "core/timer.h"
#include "core/updater.h"

namespace sets {

class SettingsBase {
    typedef std::function<void(Builder& b)> BuildCallback;
    typedef std::function<void(Updater& upd)> UpdateCallback;
    typedef std::function<void(Text path)> FileCallback;

    struct Config {
        // таймаут отправки слайдера, мс. 0 чтобы отключить
        uint16_t sliderTout = 100;

        // таймаут ожидания ответа сервера, мс
        uint16_t requestTout = 2000;

        // период обновлений, мс. 0 чтобы отключить
        uint16_t updateTout = 2500;
    };

    struct CustomJS {
        const char* p = nullptr;
        size_t len;
        uint8_t hash = 0;
        bool isFile;
        bool gz;
    };

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
        config.updateTout = prd;
    }

// подключить базу данных
#ifndef SETT_NO_DB
    void attachDB(GyverDB* db) {
        _db = db;
    }
#endif

    // использовать автоматические обновления из БД (при изменении записи новое значение отправится в браузер) (умолч. true)
    void useAutoUpdates(bool use) {
        _db_update = use;
#ifndef SETT_NO_DB
        if (_db) _db->useUpdates(use);
#endif
    }

    // обработчик билда типа f(sets::Builder& b)
    void onBuild(BuildCallback cb) {
        _build_cb = cb;
    }

    // обработчик обновлений типа f(sets::Updater& upd)
    void onUpdate(UpdateCallback cb) {
        _upd_cb = cb;
    }

    // обработчик скачивания файлов с устройства типа f(Text path)
    void onFetch(FileCallback cb) {
        fetch_cb = cb;
    }

    // обработчик загрузки файлов на устройство типа f(Text path)
    void onUpload(FileCallback cb) {
        upload_cb = cb;
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

    // установить размер пакета (умолч. 1024 Б). 0 - отключить разбивку на пакеты
    void setPacketSize(size_t size) {
        _packet_size = size;
    }

    // установить кастом js код из PROGMEM
    void setCustom(const char* js, size_t len, bool gz = false) {
        custom.isFile = false;
        custom.p = js;
        custom.gz = gz;
        custom.len = len;
        custom.hash = 0;
        while (len--) custom.hash += pgm_read_byte(js++);
    }

    // установить кастом js код из файла
    void setCustomFile(const char* path, bool gz = false) {
        custom.isFile = true;
        custom.p = path;
        custom.gz = gz;
        custom.hash = 0;
        File f = sets::FS.openRead(custom.p);
        if (f) {
            while (f.available()) custom.hash += f.read();
        }
    }

    // установить инфо о проекте (отображается на вкладке настроек и файлов)
    void setProjectInfo(const char* name, const char* link = nullptr) {
        _pname = name;
        _plink = link;
    }

    // настройки вебморды
    Config config;

   protected:
    CustomJS custom;
    FileCallback fetch_cb = nullptr;
    FileCallback upload_cb = nullptr;

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
                    if (_db_update) _db->useUpdates(false);
                    _db->update(id, value);
                    if (_db_update) _db->useUpdates(true);
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
                    p('{');
                    p[Code::type] = Code::update;
                    p[Code::rssi] = constrain(2 * (WiFi.RSSI() + 100), 0, 100);
                    if (p[Code::content]('[')) {
#ifndef SETT_NO_DB
                        if (_db && _db_update) {
                            while (_db->updatesAvailable()) {
                                size_t id = _db->updateNext();
                                p('{');
                                p[Code::id] = id;
                                p[Code::data];
                                p.addFromDB(_db, id);
                                p('}');
                            }
                        }
#endif
                        if (_upd_cb) _upd_cb(upd);
                        p(']');
                    }
                    p('}');

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

            case SH("create"):
                if (granted) {
                    FS.openWrite(value);
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
    size_t _packet_size = 1024;
#ifndef SETT_NO_DB
    GyverDB* _db = nullptr;
#endif
    bool _db_update = true;
    bool _rst = false;
    bool _reload = false;
    const char *_pname = nullptr, *_plink = nullptr;

    void _answerEmpty() {
        uint8_t p;
        send(&p, 0);
    }

    void _sendFs(bool granted) {
        String str;
        if (granted) FS.listDir(str, "/", true);

        Packet p;
        p('{');
        p[Code::type] = Code::fs;
        p[Code::content] = str;
        p[Code::used] = FS.usedSpace();
        p[Code::total] = FS.totalSpace();
        if (!granted) p[Code::error] = F("Access denied");
        p('}');
        send(p.buf(), p.length());
    }

    void _sendBuild(bool granted) {
        if (_build_cb) {
            Packet p(_packet_size, this, _hook);
            p('{');
            p[Code::type] = Code::build;
            p[Code::update_tout] = config.updateTout;
            p[Code::request_tout] = config.requestTout;
            p[Code::slider_tout] = config.sliderTout;
            p[Code::rssi] = constrain(2 * (WiFi.RSSI() + 100), 0, 100);
            if (custom.p) p[Code::custom_hash] = custom.hash;
            if (_title.length()) p[Code::title] = _title;
            if (_passh) p[Code::granted] = granted;
            if (_pname) p[Code::proj_name] = _pname;
            if (_plink) p[Code::proj_link] = _plink;
#ifdef ATOMIC_FS_UPDATE
            p[Code::gzip] = true;
#endif
            if (p[Code::content]('[')) {
                Build action(Build::Type::Build, granted);
#ifndef SETT_NO_DB
                Builder builder(this, action, &p, _db);
#else
                Builder builder(this, action, &p);
#endif
                _build_cb(builder);
                p(']');
            }
            p('}');
            send(p.buf(), p.length());
        } else {
            _answerEmpty();
        }
    }

    static void _hook(void* settptr, Packet& p) {
        static_cast<SettingsBase*>(settptr)->send(p.buf(), p.length());
    }
};

}  // namespace sets