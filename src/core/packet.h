#pragma once
#include <Arduino.h>
#include <BSON.h>

#ifndef SETT_NO_DB
#include <GyverDB.h>
#endif

#include "codes.h"
#include "logger.h"

#define PACKET_OVERLAP 128

namespace sets {

class Packet;
typedef void (*SendHook)(void* settptr, Packet& packet);

class Packet : public BSON {
   public:
    Packet() {}
    Packet(size_t maxsize, void* settptr, SendHook hook) : _max_size(maxsize), _settptr(settptr), _hook(hook) {
        reserve(maxsize + PACKET_OVERLAP);
    }

    using BSON::add;
    using BSON::operator+=;
    using BSON::operator=;
    using BSON::operator[];

    void add(Text t) { addStr(t.str(), t.length(), t.pgm()); }
    Packet& operator[](Text t) { return add(t), *this; }
    void operator=(Text t) { add(t); }
    void operator+=(Text t) { add(t); }

    void add(Code code) { addCode(code); }
    Packet& operator[](Code key) { return add(key), *this; }
    void operator=(Code code) { add(code); }
    void operator+=(Code code) { add(code); }

    void addLogger(Logger& log) {
        if (log.buffer[log.size() - 1]) {  // filled
            beginStr(log.size());
            write(log.buffer + log.getHead(), log.size() - log.getHead());
            write(log.buffer, log.getHead());
        } else {
            beginStr(log.getHead());
            write(log.buffer, log.getHead());
        }
    }

    bool inDB(void* db, size_t hash) {
#ifndef SETT_NO_DB
        return static_cast<GyverDB*>(db)->has(hash);
#endif
        return false;
    }

    void addFromDB(void* db, size_t hash) {
#ifndef SETT_NO_DB
        gdb::Entry en = static_cast<GyverDB*>(db)->get(hash);
        switch (en.type()) {
            case gdb::Type::Int:
            case gdb::Type::Int64:
                addInt(en.buffer(), en.size());
                break;

            case gdb::Type::Uint:
            case gdb::Type::Uint64:
                addUint(en.buffer(), en.size());
                break;

            case gdb::Type::Float:
                addFloat(en.buffer());
                break;

            case gdb::Type::String:
                addStr((const char*)en.buffer(), en.size());
                break;

            default:
                addNull();
                break;
        }
#endif
    }

    void checkLen() {
        if (_max_size && _hook && length() > _max_size) {
            _hook(_settptr, *this);
            clear();
        }
    }

    void concatString(Text str) {
        write(str.str(), str.length(), str.pgm());
    }

   private:
    size_t _max_size = 0;
    void* _settptr = nullptr;
    SendHook _hook = nullptr;
};

}  // namespace sets