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

    void addLogger(Logger& log) {
        if (log.buffer[log.size() - 1]) {  // filled
            beginStr(log.size());
            concat((uint8_t*)log.buffer + log.getHead(), log.size() - log.getHead());
            concat((uint8_t*)log.buffer, log.getHead());
        } else {
            beginStr(log.getHead());
            concat((uint8_t*)log.buffer, log.getHead());
        }
    }

    void addFromDB(void* dbp, size_t hash) {
#ifndef SETT_NO_DB
        GyverDB* db = (GyverDB*)dbp;
        gdb::Entry en = db->get(hash);
        switch (en.type()) {
            case gdb::Type::Int:
                add(en.toInt());
                break;
            case gdb::Type::Uint:
                add((uint32_t)en.toInt());
                break;

            case gdb::Type::Int64:
                add(en.toInt64());
                break;
            case gdb::Type::Uint64:
                add((uint64_t)en.toInt64());
                break;

            case gdb::Type::Float:
                add(en.toFloat(), 4);
                break;

            case gdb::Type::String:
                add((Text)en);
                break;

            default:
                add(0u);
        }
#endif
    }

    void checkLen() {
        if (_max_size && _hook && length() > _max_size) {
            _hook(_settptr, *this);
            clear();
        }
    }

   private:
    size_t _max_size = 0;
    void* _settptr = nullptr;
    SendHook _hook = nullptr;
};

}  // namespace sets