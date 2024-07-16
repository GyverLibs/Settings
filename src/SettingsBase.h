#pragma once
#include <Arduino.h>
#include <GyverDB.h>
#include <StringUtils.h>

#include "core/builder.h"
#include "core/colors.h"
#include "core/containers.h"
#include "core/packet.h"
#include "core/timer.h"
#include "core/updater.h"

#define SETS_RESERVE 512

class SettingsBase {
    typedef std::function<void(sets::Builder& b)> BuildCallback;
    typedef std::function<void(sets::Updater& upd)> UpdateCallback;

   public:
    SettingsBase(const String& title = "", GyverDB* db = nullptr) : _title(title), _db(db) {}

    // установить заголовок страницы
    void setTitle(const String& title) {
        _title = title;
    }

    // установить период обновлений (умолч. 2500мс), 0 чтобы отключить
    void setUpdatePeriod(uint16_t prd) {
        _updPeriod = prd;
    }

    // подключить базу данных
    void attachDB(GyverDB* db) {
        _db = db;
    }

    // использовать автоматические обновления из БД (при изменении записи новое значение отправится в браузер)
    void useAutoUpdates(bool use) {
        _dbupdates = use;
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
        if (_db) _db->tick();
    }

   protected:
    // отправка для родительского класса
    virtual void send(uint8_t* data, size_t len) {}

    // парсить запрос клиента
    void parse(Text action, Text idtxt, Text value) {
        size_t id = idtxt.toInt32HEX();

        switch (action.hash()) {
            case SH("load"):
                _sendBuild();
                break;

            case SH("set"):
                if (_db) {
                    if (_dbupdates) _db->useUpdates(false);
                    _db->update(id, value);
                    if (_dbupdates) _db->useUpdates(true);
                }
                if (_build_cb) {
                    sets::Build action(sets::Build::Type::Set, id, value);
                    sets::Builder b(action);
                    _build_cb(b);
                    if (b.isReload()) _sendBuild();
                    else _answerEmpty();
                } else {
                    _answerEmpty();
                }
                break;

            case SH("click"):
                if (_build_cb) {
                    sets::Build action(sets::Build::Type::Click, id);
                    sets::Builder b(action);
                    _build_cb(b);
                    if (b.isReload()) _sendBuild();
                    else _answerEmpty();
                } else {
                    _answerEmpty();
                }
                break;

            case SH("ping"):
                if (_upd_cb || _db) {
                    sets::Packet p;
                    sets::Updater upd(p);
                    p.beginObj();
                    p.addCode(sets::Code::type, sets::Code::update);
                    p.beginArr(sets::Code::content);
                    if (_db && _dbupdates) {
                        while (_db->updatesAvailable()) {
                            size_t id = _db->updateNext();
                            p.beginObj();
                            p.addUint(sets::Code::id, id);
                            p.addKey(sets::Code::value);
                            p.addFromDB(_db, id);
                            p.endObj();
                        }
                    }
                    if (_upd_cb) _upd_cb(upd);
                    p.endArr();
                    p.endObj();
                    send(p.buf(), p.length());
                } else {
                    _answerEmpty();
                }
                break;
        }
    }

   private:
    BuildCallback _build_cb = nullptr;
    UpdateCallback _upd_cb = nullptr;
    String _title;
    GyverDB* _db = nullptr;
    uint16_t _updPeriod = 2500;
    bool _dbupdates = true;

    void _answerEmpty() {
        uint8_t p;
        send(&p, 0);
    }

    void _sendBuild() {
        if (_build_cb) {
            sets::Packet p;
            p.reserve(SETS_RESERVE);
            p.beginObj();
            p.addCode(sets::Code::type, sets::Code::build);
            p.addUint(sets::Code::ping, _updPeriod);
            if (_title.length()) p.addText(sets::Code::title, _title);
            p.beginArr(sets::Code::content);
            sets::Build action(sets::Build::Type::Build);
            sets::Builder builder(action, _db, &p);
            _build_cb(builder);
            p.endArr();
            p.endObj();
            send(p.buf(), p.length());
        } else {
            _answerEmpty();
        }
    }
};