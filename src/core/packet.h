#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <GyverDB.h>

#include "codes.h"

namespace sets {

class Packet : public BSON {
   public:
    void addCode(Code key, Code val) {
        addCode((uint8_t)key, (uint8_t)val);
    }
    void addCode(Code val) {
        addCode((uint8_t)val);
    }
    void addBool(Code key, bool b) {
        addBool((uint8_t)key, b);
    }
    template <typename T>
    void addInt(Code key, T val) {
        addInt((uint8_t)key, val);
    }
    template <typename T>
    void addUint(Code key, T val) {
        addUint((uint8_t)key, val);
    }
    void addFloat(Code key, float f, uint8_t dec = 3) {
        addFloat((uint8_t)key, f, dec);
    }
    void addText(Code key, Text text) {
        addText((uint8_t)key, text);
    }
    void addKey(Code key) {
        addKey((uint8_t)key);
    }
    void beginObj(Code key) {
        beginObj((uint8_t)key);
    }
    void beginArr(Code key) {
        beginArr((uint8_t)key);
    }

    void addFromDB(GyverDB* db, size_t hash) {
        gdb::Entry en = db->get(hash);
        switch (en.type()) {
            case gdb::Type::Int8:
                addInt(en.toInt8());
                break;
            case gdb::Type::Uint8:
                addUint((uint8_t)en.toInt8());
                break;

            case gdb::Type::Int16:
                addInt(en.toInt16());
                break;
            case gdb::Type::Uint16:
                addUint((uint16_t)en.toInt16());
                break;

            case gdb::Type::Int32:
                addInt(en.toInt32());
                break;
            case gdb::Type::Uint32:
                addUint((uint32_t)en.toInt32());
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
                addInt(false);
        }
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