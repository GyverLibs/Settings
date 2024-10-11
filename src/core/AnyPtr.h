#pragma once
#include <Arduino.h>

#include "packet.h"

class AnyPtr {
    enum class Type {
        None,
        Text,
        ConstChar,
        ConstFstr,
        Char,
        String,
        Int8,
        Int16,
        Int32,
        Float,
        Double,
    };

   public:
    AnyPtr() {}
    AnyPtr(std::nullptr_t p) {}
    AnyPtr(const char* p) : p((void*)p), type(Type::ConstChar) {}
    AnyPtr(const __FlashStringHelper* p) : p((void*)p), type(Type::ConstFstr) {}
    AnyPtr(char* p, size_t len) : p(p), len(len), type(Type::Char) {}
    AnyPtr(String* p) : p(p), type(Type::String) {}
    AnyPtr(Text* p) : p(p), type(p->valid() ? Type::Text : Type::None) {}
    AnyPtr(float* p) : p(p), type(Type::Float) {}
    AnyPtr(double* p) : p(p), type(Type::Double) {}

    template <typename T>
    AnyPtr(T* p) : p(p) {
        switch (sizeof(T)) {
            case 1: type = Type::Int8; break;
            case 2: type = Type::Int16; break;
            case 4: type = Type::Int32; break;
        }
    }

    operator bool() {
        return p && type != Type::None;
    }

    void write(sets::Packet* pkt) {
        switch (type) {
            case Type::ConstChar: pkt->addText((const char*)p); break;
            case Type::ConstFstr: pkt->addText((const __FlashStringHelper*)p); break;

            case Type::Text: pkt->addText(*((Text*)p)); break;
            case Type::String: pkt->addText(*((String*)p)); break;
            case Type::Char: pkt->addText((char*)p); break;

            case Type::Int8: pkt->addInt(*((int8_t*)p)); break;
            case Type::Int16: pkt->addInt(*((int16_t*)p)); break;
            case Type::Int32: pkt->addInt(*((int32_t*)p)); break;

            case Type::Float: pkt->addFloat(*((float*)p), 4); break;
            case Type::Double: pkt->addFloat(*((double*)p), 4); break;

            default: break;
        }
    }

    void read(const Text& value) {
        switch (type) {
            case Type::String: value.toString(*((String*)p)); break;
            case Type::Char: value.toStr((char*)p, len); break;

            case Type::Int8: *((int8_t*)p) = value.toInt(); break;
            case Type::Int16: *((int16_t*)p) = value.toInt(); break;
            case Type::Int32: *((int32_t*)p) = value.toInt(); break;

            case Type::Float: *((float*)p) = value.toFloat(); break;
            case Type::Double: *((double*)p) = value.toFloat(); break;

            default: break;
        }
    }

   private:
    void* p = nullptr;
    size_t len = 0;
    Type type = Type::None;
};