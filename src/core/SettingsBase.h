#pragma once
#include <Arduino.h>
#include <LittleFS.h>
#include <StampKeeper.h>
#include <StringUtils.h>

#ifndef SETT_NO_DB
#include <GyverDB.h>
#endif

#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif

#include "../web/settings.h"
#include "./HybridFS.h"
#include "./builder.h"
#include "./colors.h"
#include "./containers.h"
#include "./logger.h"
#include "./macro.h"
#include "./packet.h"
#include "./updater.h"

namespace sets {

class SettingsBase {
    static const uint16_t FOCUS_TOUT = 5000;
    static const uint16_t DB_WS_UPDATE_PRD = 300;

   protected:
    typedef std::function<void(Builder& b)> BuildCallback;
    typedef std::function<void(Updater& upd)> UpdateCallback;
    typedef std::function<void(Text path)> FileCallback;
    typedef std::function<void()> FocusCallback;

   private:
    struct Config {
        // таймаут отправки слайдера, мс. 0 чтобы отключить
        uint16_t sliderTout = 100;

        // таймаут ожидания ответа сервера, мс
        uint16_t requestTout = 2000;

        // период обновлений, мс. 0 чтобы отключить
        uint16_t updateTout = 2500;

        // период пинга, мс. 0 чтобы отключить
        uint16_t pingTout = 2500;

        // основная цветовая схема
        Colors theme = Colors::Green;

        // использовать файловый менеджер
        bool useFS = true;
    };

    struct CustomJS {
        const char* p = nullptr;
        size_t len;
        uint8_t hash = 0;
        bool isFile;
        bool gz;
    };

    class Timer {
       public:
        bool elapsed(uint32_t prd) {
            return (_tmr && millis() - _tmr >= prd);
        }
        void restart() {
            _tmr = millis();
            if (!_tmr) --_tmr;
        }
        void stop() {
            _tmr = 0;
        }
        bool running() {
            return _tmr;
        }

       private:
        uint32_t _tmr = 0;
    };

    class InlineUpdater : public Updater {
       public:
        InlineUpdater(SettingsBase& sets) : Updater(p), sets(sets) {
            p('{');
            p[Code::type] = Code::update;
            p[Code::content]('[');
        }

        InlineUpdater(InlineUpdater& u) = default;
        InlineUpdater(InlineUpdater&& u) = default;
        InlineUpdater& operator=(InlineUpdater&&) = default;
        InlineUpdater& operator=(InlineUpdater&) = default;

        ~InlineUpdater() {
            p(']');
            p('}');
            if (sets.focused()) sets._sendWS(p);
        }

       private:
        SettingsBase& sets;
        Packet p;
    };

   protected:
    // начать обновления [ДЛЯ ВЕРСИЙ С ВЕБСОКЕТОМ]
    InlineUpdater updater() {
        return InlineUpdater(*this);
    }

   public:
#ifndef SETT_NO_DB
    SettingsBase(const String& title = "", GyverDB* db = nullptr) : _title(title), _db(db) {
#else
    SettingsBase(const String& title = "") : _title(title) {
#endif
        useAutoUpdates(true);
        fs.setFS(LittleFS);
    }

    // файловая система
    HybridFS fs;

    // установить версию прошивки для отображения в меню
    void setVersion(const char* ver) {
        _f_ver = ver;
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

    // обработчик обновлений типа f(sets::Updater& upd) [ДЛЯ ВЕРСИЙ БЕЗ ВЕБСОКЕТА]
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

    // обработчик подключения браузера f(bool focus)
    void onFocusChange(FocusCallback cb) {
        _focus_cb = cb;
    }

    // тикер, вызывать в родительском классе
    void tick() {
#ifndef SETT_NO_DB
        if (_db) _db->tick();
        if (_upd_tmr.elapsed(DB_WS_UPDATE_PRD) && _dbHasUpdates()) {
            _upd_tmr.restart();
            Packet p;
            p('{');
            p[Code::type] = Code::update;
            p[Code::content]('[');
            _fillUpdates(p);
            p(']');
            p('}');
            _sendWS(p);
        }
#endif
        if (_rst) {
            delay(2000);
            ESP.restart();
        }
        if (_focus_tmr.elapsed(FOCUS_TOUT)) {
            _focus_tmr.stop();
            _upd_tmr.stop();
            if (_focus_cb) _focus_cb();
        }
        rtc.tick();
    }

    // перезагрузить страницу. Можно вызывать где угодно + в обработчике update
    void reload(bool force = false) {
        if (_upd_tmr.running()) {
            Packet p;
            p('{');
            p[Code::type] = Code::reload;
            if (force) p[Code::force] = true;
            p('}');
            _sendWS(p);
        } else {
            _reload = force ? -1 : 1;
        }
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
        File f = fs.openRead(custom.p);
        if (f) {
            while (f.available()) custom.hash += f.read();
        }
    }

    // установить инфо о проекте (отображается на вкладке настроек и файлов)
    void setProjectInfo(const char* name, const char* link = nullptr) {
        _pname = name;
        _plink = link;
    }

    // вебморда открыта в браузере
    bool focused() {
        return _focus_tmr.running();
    }

    // настройки вебморды
    Config config;

    // время с браузера
    StampKeeper rtc;

   protected:
    CustomJS custom;
    FileCallback fetch_cb = nullptr;
    FileCallback upload_cb = nullptr;

    virtual String getMac() { return String(); }
    virtual int getRSSI() { return 100; }
    virtual IPAddress getIP() { return IPAddress(); }

    // ответ HTTP
    virtual void answer(uint8_t* data, size_t len) = 0;

    // отправка WS
    virtual void sendWS(uint8_t* data, size_t len, bool broadcast) {}

    bool authenticate(size_t passh) {
        return !_passh || (_passh == passh);
    }

    void restart() {
        _rst = true;
    }

    struct __attribute__((packed)) WSHeader {
        uint16_t pid;
        uint32_t auth;
        uint32_t action;
        uint32_t id;
    };

    void setWSPort(uint16_t port) {
        _ws_port = port;
    }

    // парсить запрос WS клиента
    void parseWS(uint8_t* data, size_t len) {
        _upd_tmr.restart();
        if (len < sizeof(WSHeader)) return;

        WSHeader header;
        memcpy(&header, data, sizeof(WSHeader));
        Text value(data + sizeof(WSHeader), len - sizeof(WSHeader));

        _headerP = &header;
        parse(header.auth, header.action, header.id, value);
        _headerP = nullptr;
    }

    // парсить запрос HTTP клиента
    void parse(size_t passh, size_t actionh, size_t idh, Text value) {
        if (!_focus_tmr.running()) {
            _focus_tmr.restart();
            if (_focus_cb) _focus_cb();
        }
        _focus_tmr.restart();

        bool granted = authenticate(passh);

        switch (actionh) {
            case SH("discover"): {
                Packet p;
                p.concatString(F(R"raw({"type":"discover","name":")raw"));
                if (_title.length()) p.concatString(_title);
                else p.concatString(F("Unnamed"));
                p.concatString(F(R"raw(","mac":")raw"));
                p.concatString(getMac());
                p.concatString("\"}");
                _answer(p);
                return;
            } break;

            case SH("load"):
#ifndef SETT_NO_DB
                if (_dbHasUpdates()) _db->skipUpdates();
#endif
                rtc.sync(value.toInt32HEX());
                _sendBuild(granted);
                return;

            case SH("unfocus"):
                _focus_tmr.stop();
                if (_focus_cb) _focus_cb();
                break;

            case SH("menu"):
                if (_build_cb) {
                    Build action(Build::Type::Menu, granted, idh);
                    Builder b(this, action);
                    _build_cb(b);
                }
                break;

            case SH("set"):
            case SH("click"):
#ifndef SETT_NO_DB
                if (_db && actionh == SH("set")) {
                    if (_db_update) _db->useUpdates(false);
                    _db->update(idh, value);
                    if (_db_update) _db->useUpdates(true);
                }
#endif
                if (_build_cb) {
                    Build action(Build::Type::Set, granted, idh, value);
                    Builder b(this, action);
                    _build_cb(b);
                    if (b.isReload()) _reload = b.isReload();
                    if (_reload) {
                        _sendReload();
                        return;
                    }
                }
                if (actionh == SH("set") && _headerP) {
                    Packet p;
                    p('{');
                    p[Code::type] = Code::update;
                    p[Code::content]('[');

                    p('{');
                    p[Code::id] = idh;
                    p[Code::data] = value;
                    p('}');

                    p(']');
                    p('}');
                    _send(p, true, true);
                    return;
                }
                break;

            case SH("update"):
                if (_reload) {
#ifndef SETT_NO_DB
                    if (_dbHasUpdates()) _db->skipUpdates();
#endif
                    _sendReload();
                    return;
                } else if (_dbHasUpdates() || _upd_cb) {
                    Packet p;
                    p('{');
                    p[Code::type] = Code::update;
                    p[Code::rssi] = getRSSI();
                    p[Code::content]('[');
                    _fillUpdates(p);
                    if (_upd_cb) {
                        Updater upd(p);
                        _upd_cb(upd);
                    }
                    p(']');
                    p('}');
                    _answer(p);
                    return;
                }
                break;

            case SH("fs"):
                _sendFs(granted);
                return;

            case SH("remove"):
                if (granted) {
                    fs.remove(value.c_str());
                    _sendFs(true);
                    return;
                }
                break;

            case SH("create"):
                if (granted) {
                    fs.openWrite(value.c_str());
                    _sendFs(true);
                    return;
                }
                break;

            case SH("ping"): {
                BSON b;
                b('{');
                b[Code::rssi] = getRSSI();
                b('}');
                _answer(b);
                return;
            }

        }  // switch
        _answerEmpty();
    }

   private:
    BuildCallback _build_cb = nullptr;
    UpdateCallback _upd_cb = nullptr;
    FocusCallback _focus_cb = nullptr;
    const char* _f_ver = nullptr;
    String _title;
    Timer _focus_tmr;
    Timer _upd_tmr;
    size_t _passh = 0;
    size_t _packet_size = 1024;
#ifndef SETT_NO_DB
    GyverDB* _db = nullptr;
#endif
    const char *_pname = nullptr, *_plink = nullptr;
    WSHeader* _headerP = nullptr;
    uint16_t _ws_port = 0;
    bool _db_update = true;
    bool _rst = false;
    int8_t _reload = 0;

    void _answerEmpty() {
        BSON b;
        _answer(b);
    }

    void _sendFs(bool granted) {
        String res;
        if (granted) fs.listDir(res, "/", true);

        Packet p;
        p('{');
        p[Code::type] = Code::fs;
        p[Code::content] = res;
        p[Code::used] = fs.flash.usedSpace();
        p[Code::total] = fs.flash.totalSpace();
        if (!granted) p[Code::error] = F("Access denied");
        p('}');
        _answer(p);
    }

    void _sendBuild(bool granted) {
        if (_build_cb) {
            Packet p(_packet_size, this, _hook);
            p('{');
            p[Code::type] = Code::build;
            p[Code::ws_port] = _ws_port;
            p[Code::update_tout] = config.updateTout;
            p[Code::ping_tout] = config.pingTout;
            p[Code::request_tout] = config.requestTout;
            p[Code::send_tout] = config.sliderTout;
            p[Code::use_fs] = config.useFS;
            p[Code::color] = (uint32_t)config.theme;
            p[Code::rssi] = getRSSI();
            p[Code::uptime] = millis() / 1000;
            p[Code::mac] = getMac();
            p[Code::local_ip] = getIP().toString();
            p[Code::s_ver] = SETTINGS_VER;
            if (_f_ver) p[Code::f_ver] = _f_ver;
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
            _answer(p);
        } else {
            _answerEmpty();
        }
    }

    bool _dbHasUpdates() {
#ifndef SETT_NO_DB
        return _db && _db_update && _db->updatesAvailable();
#endif
        return false;
    }

    void _fillUpdates(Packet& p) {
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
    }

    void _sendReload() {
        Packet p;
        p('{');
        p[Code::type] = Code::reload;
        if (_reload < 0) p[Code::force] = true;
        p('}');
        _answer(p);
        _reload = 0;
    }

    void _answer(BSON& bson) {
        _headerP ? _send(bson, false, false) : answer(bson.buf(), bson.length());
    }

    void _send(BSON& bson, bool broadcast, bool skip) {
        bson.write(&skip, 1);
        if (_headerP) {
            bson.write(&_headerP->pid, 2);
        } else {
            uint16_t pid = 0;
            bson.write(&pid, 2);
        }
        sendWS(bson.buf(), bson.length(), broadcast);
    }
    void _sendWS(BSON& bson) {
        uint32_t p = 0;
        bson.write(&p, 3);  // skip 0, pid 0
        sendWS(bson.buf(), bson.length(), true);
    }

    static void _hook(void* settptr, Packet& p) {
        static_cast<SettingsBase*>(settptr)->_answer(p);
    }
};

}  // namespace sets