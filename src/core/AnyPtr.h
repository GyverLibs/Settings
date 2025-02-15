#pragma once
#include <Arduino.h>

#include "packet.h"
#include "pos.h"

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
        Uint8,
        Int16,
        Uint16,
        Int32,
        Uint32,
        Int64,
        Uint64,
        Float,
        Double,
        Pos,
    };

    AnyPtr() {}
    AnyPtr(std::nullptr_t p) {}
    AnyPtr(const char* p) : _p((void*)p), _type(Type::ConstChar) {}
    AnyPtr(const __FlashStringHelper* p) : _p((void*)p), _type(Type::ConstFstr) {}
    AnyPtr(char* p, size_t _len) : _p(p), _len(_len), _type(Type::Char) {}
    AnyPtr(String* p) : _p(p), _type(Type::String) {}
    AnyPtr(Text* p) : _p(p), _type(p->valid() ? Type::Text : Type::None) {}
    AnyPtr(float* p) : _p(p), _type(Type::Float) {}
    AnyPtr(double* p) : _p(p), _type(Type::Double) {}
    AnyPtr(bool* p) : _p(p), _type(Type::Bool) {}
    AnyPtr(sets::Pos* p) : _p(p), _type(Type::Pos) {}

    template <typename T>
    AnyPtr(T* p) : _p(p) {
        switch (sizeof(T)) {
            case 1: _type = *p < 0 ? Type::Int8 : Type::Uint8; break;
            case 2: _type = *p < 0 ? Type::Int16 : Type::Uint16; break;
            case 4: _type = *p < 0 ? Type::Int32 : Type::Uint32; break;
            case 8: _type = *p < 0 ? Type::Int64 : Type::Uint64; break;
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

            case Type::Uint8: *pkt += *((uint8_t*)_p); break;
            case Type::Uint16: *pkt += *((uint16_t*)_p); break;
            case Type::Uint32: *pkt += *((uint32_t*)_p); break;
            case Type::Uint64: *pkt += *((uint64_t*)_p); break;

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
            case Type::Int8:
            case Type::Uint8: *((uint8_t*)_p) = value.toInt(); break;
            case Type::Int16:
            case Type::Uint16: *((uint16_t*)_p) = value.toInt(); break;
            case Type::Int32:
            case Type::Uint32: *((uint32_t*)_p) = value.toInt(); break;
            case Type::Int64:
            case Type::Uint64: *((uint64_t*)_p) = value.toInt64(); break;

            case Type::Float: *((float*)_p) = value.toFloat(); break;
            case Type::Double: *((double*)_p) = value.toFloat(); break;

            case Type::Pos: {
                sets::Pos* p = (sets::Pos*)_p;
                uint32_t v = value;
                p->x = int16_t(v >> 16);
                p->y = int16_t(v & 0xffff);
                p->_changed = true;
            } break;

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