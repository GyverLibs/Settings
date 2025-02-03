#pragma once
#include <Arduino.h>

#include "packet.h"

class AnyPtr {
   public:
    enum class Type {
        None,
        Text,
        ConstChar,
        ConstFstr,
        Char,
        String,
        Bool,
        Int8,
        Int16,
        Int32,
        Int64,
        Float,
        Double,
    };

    AnyPtr() {}
    AnyPtr(std::nullptr_t _p) {}
    AnyPtr(const char* _p) : _p((void*)_p), _type(Type::ConstChar) {}
    AnyPtr(const __FlashStringHelper* _p) : _p((void*)_p), _type(Type::ConstFstr) {}
    AnyPtr(char* _p, size_t _len) : _p(_p), _len(_len), _type(Type::Char) {}
    AnyPtr(String* _p) : _p(_p), _type(Type::String) {}
    AnyPtr(Text* _p) : _p(_p), _type(_p->valid() ? Type::Text : Type::None) {}
    AnyPtr(float* _p) : _p(_p), _type(Type::Float) {}
    AnyPtr(double* _p) : _p(_p), _type(Type::Double) {}
    AnyPtr(bool* _p) : _p(_p), _type(Type::Bool) {}

    template <typename T>
    AnyPtr(T* _p) : _p(_p) {
        switch (sizeof(T)) {
            case 1: _type = Type::Int8; break;
            case 2: _type = Type::Int16; break;
            case 4: _type = Type::Int32; break;
            case 8: _type = Type::Int64; break;
        }
    }

    operator bool() {
        return _p && _type != Type::None;
    }

    void write(sets::Packet* pkt) {
        switch (_type) {
            case Type::ConstChar: *pkt += (const char*)_p; break;
            case Type::ConstFstr: *pkt += (const __FlashStringHelper*)_p; break;

            case Type::Text: *pkt += *((Text*)_p); break;
            case Type::String: *pkt += *((String*)_p); break;
            case Type::Char: *pkt += (char*)_p; break;

            case Type::Bool: *pkt += *((bool*)_p); break;
            case Type::Int8: *pkt += *((int8_t*)_p); break;
            case Type::Int16: *pkt += *((int16_t*)_p); break;
            case Type::Int32: *pkt += *((int32_t*)_p); break;
            case Type::Int64: *pkt += *((int64_t*)_p); break;

            case Type::Float: *pkt += *((float*)_p); break;
            case Type::Double: *pkt += *((double*)_p); break;

            default: break;
        }
    }

    void read(const Text& value) {
        switch (_type) {
            case Type::String: value.toString(*((String*)_p)); break;
            case Type::Char: value.toStr((char*)_p, _len); break;

            case Type::Bool: *((bool*)_p) = value.toBool(); break;
            case Type::Int8: *((int8_t*)_p) = value.toInt(); break;
            case Type::Int16: *((int16_t*)_p) = value.toInt(); break;
            case Type::Int32: *((int32_t*)_p) = value.toInt(); break;
            case Type::Int64: *((int64_t*)_p) = value.toInt64(); break;

            case Type::Float: *((float*)_p) = value.toFloat(); break;
            case Type::Double: *((double*)_p) = value.toFloat(); break;

            default: break;
        }
    }

    size_t len() {
        return _len;
    }

    Type type() {
        return _type;
    }

   private:
    void* _p = nullptr;
    size_t _len = 0;
    Type _type = Type::None;
};