#pragma once
#include <Arduino.h>
#include <lbuf/lbuf_ext.h>
#include <ptr/ptr_uniq.h>

namespace sets {

class Logger : public gtl::lbuf_ext<char>, public Print {
   public:
    Logger(size_t size) : _p(new char[size]()) {
        setBuffer(_p.get(), size);
    }

    static String error() {
        return "err: ";
    }
    static String warn() {
        return "warn: ";
    }
    static String info() {
        return "info: ";
    }

    // вывод в String
    String toString() {
        String s;
        if (buffer[size() - 1]) {  // filled
            s.reserve(size());
            s.concat(buffer + getHead(), size() - getHead());
            s.concat(buffer, getHead());
        } else {
            s.reserve(getHead());
            s.concat(buffer, getHead());
        }
        return s;
    }

    size_t write(uint8_t v) {
        if (!_change_f) _change_f = true;
        if (_p) gtl::lbuf_ext<char>::write(v);
        return 1;
    }

    void clear() {
        gtl::lbuf_ext<char>::clear();
        memset(_p.get(), 0, gtl::lbuf_ext<char>::size());
    }

    void _reset() {
        _change_f = false;
    }

    bool _changed() {
        return _change_f;
    }

   private:
    gtl::ptr_uniq<char> _p;
    bool _change_f = false;
};

}  // namespace sets