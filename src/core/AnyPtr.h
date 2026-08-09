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
        Int,
        Uint,
        Float,
        Double,
        Pos,
    };

    AnyPtr() {}
    AnyPtr(std::nullptr_t p) {}
    AnyPtr(const char* p) : _p((void*)p), _type(Type::ConstChar) {}
    AnyPtr(const __FlashStringHelper* p) : _p((void*)p), _type(Type::ConstFstr) {}
    AnyPtr(String* p) : _p(p), _type(Type::String) {}
    AnyPtr(Text* p) : _p(p), _type(p->valid() ? Type::Text : Type::None) {}
    AnyPtr(float* p) : _p(p), _type(Type::Float) {}
    AnyPtr(double* p) : _p(p), _type(Type::Double) {}
    AnyPtr(bool* p) : _p(p), _type(Type::Bool) {}
    AnyPtr(sets::Pos* p) : _p(p), _type(Type::Pos) {}

    template <size_t N>
    AnyPtr(char (&arr)[N]) : AnyPtr(arr, N) {}
    AnyPtr(char* p, size_t _len) : _p(p), _len(_len), _type(Type::Char) {}

    AnyPtr(signed char* p) : _p(p), _len(sizeof(char)), _type(Type::Int) {}
    AnyPtr(unsigned char* p) : _p(p), _len(sizeof(char)), _type(Type::Uint) {}
    AnyPtr(short* p) : _p(p), _len(sizeof(short)), _type(Type::Int) {}
    AnyPtr(unsigned short* p) : _p(p), _len(sizeof(short)), _type(Type::Uint) {}
    AnyPtr(int* p) : _p(p), _len(sizeof(int)), _type(Type::Int) {}
    AnyPtr(unsigned int* p) : _p(p), _len(sizeof(int)), _type(Type::Uint) {}
    AnyPtr(long* p) : _p(p), _len(sizeof(long)), _type(Type::Int) {}
    AnyPtr(unsigned long* p) : _p(p), _len(sizeof(long)), _type(Type::Uint) {}
    AnyPtr(long long* p) : _p(p), _len(sizeof(long long)), _type(Type::Int) {}
    AnyPtr(unsigned long long* p) : _p(p), _len(sizeof(long long)), _type(Type::Uint) {}

    operator bool() {
        return _p && _type != Type::None;
    }

    void write(sets::Packet* pkt) {
        switch (_type) {
            case Type::Char:
            case Type::ConstChar:
                pkt->addStr((const char*)_p);
                break;

            case Type::String: pkt->addStr(*(String*)_p); break;
            case Type::ConstFstr: pkt->addStr((const __FlashStringHelper*)_p); break;
            case Type::Text: {
                Text* text = (Text*)_p;
                pkt->addStr(text->str(), text->length(), text->pgm());
            } break;
            case Type::Bool: pkt->addBool(_p); break;
            case Type::Int: pkt->addInt(_p, _len); break;
            case Type::Uint: pkt->addUint(_p, _len); break;
            case Type::Float: pkt->addFloat(_p); break;
            case Type::Double: pkt->addFloat(*((double*)_p)); break;

            default: break;
        }
    }

    void read(const Text& value) {
        switch (_type) {
            case Type::String: value.toString(*(String*)_p); break;
            case Type::Char: value.toStr((char*)_p, _len); break;

            case Type::Bool: *((bool*)_p) = value.toBool(); break;

            case Type::Int:
            case Type::Uint:
                switch (_len) {
                    case 1: *((uint8_t*)_p) = value.toInt(); break;
                    case 2: *((uint16_t*)_p) = value.toInt(); break;
                    case 4: *((uint32_t*)_p) = value.toInt(); break;
                    case 8: *((uint64_t*)_p) = value.toInt64(); break;
                }
                break;

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