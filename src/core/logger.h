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
        if (!_changed) _changed = true;
        if (_p) gtl::lbuf_ext<char>::write(v);
        return 1;
    }

    void reset() {
        _changed = false;
    }

    bool changed() {
        return _changed;
    }

   private:
    gtl::ptr_uniq<char> _p;
    bool _changed = false;
};

}  // namespace sets