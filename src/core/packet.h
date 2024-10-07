#pragma once
#include <Arduino.h>
#include <GSON.h>

#ifndef SETT_NO_DB
#include <GyverDB.h>
#endif

#include "codes.h"

namespace sets {

class Packet : public BSON {
   public:
    void addKey(Code key) {
        addKey((uint16_t)key);
    }
    void addCode(Code key, Code val) {
        addCode((uint16_t)key, (uint16_t)val);
    }
    void addCode(Code val) {
        addCode((uint16_t)val);
    }

    template <typename T>
    void addInt(Code key, T val) {
        addInt((uint16_t)key, val);
    }
    template <typename T>
    void addUint(Code key, T val) {
        addUint((uint16_t)key, val);
    }

    void addBool(Code key, bool b) {
        addBool((uint16_t)key, b);
    }
    void addFloat(Code key, float f, uint16_t dec = 3) {
        addFloat((uint16_t)key, f, dec);
    }

    void addText(Code key, Text text) {
        addText((uint16_t)key, text);
    }

    void beginObj(Code key) {
        beginObj((uint16_t)key);
    }
    void beginArr(Code key) {
        beginArr((uint16_t)key);
    }

    void addFromDB(void* dbp, size_t hash) {
#ifndef SETT_NO_DB
        GyverDB* db = (GyverDB*)dbp;
        gdb::Entry en = db->get(hash);
        switch (en.type()) {
            case gdb::Type::Int:
                addInt(en.toInt());
                break;
            case gdb::Type::Uint:
                addUint((uint32_t)en.toInt());
                break;

            case gdb::Type::Int64:
                addInt(en.toInt64());
                break;
            case gdb::Type::Uint64:
                addUint((uint64_t)en.toInt64());
                break;

            case gdb::Type::Float:
                addFloat(en.toFloat(), 4);
                break;

            case gdb::Type::String:
                addText(en);
                break;

            default:
                addInt(0);
        }
#endif
    }

    using BSON::addBool;
    using BSON::addCode;
    using BSON::addFloat;
    using BSON::addInt;
    using BSON::addKey;
    using BSON::addText;
    using BSON::addUint;
    using BSON::beginArr;
    using BSON::beginObj;
};

}  // namespace sets