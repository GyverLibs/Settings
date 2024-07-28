#pragma once
#include <Arduino.h>

#include "packet.h"

namespace sets {

class Updater {
   public:
    Updater(Packet& p) : p(p) {}

    // пустой апдейт (например для вызова Confirm)
    void update(size_t id) {
        p.beginObj();
        p.addUint(sets::Code::id, id);
        p.endObj();
    }

    // апдейт с текстом
    void update(size_t id, Text value) {
        p.beginObj();
        p.addUint(sets::Code::id, id);
        p.addText(sets::Code::value, value);
        p.endObj();
    }

    // апдейт с float
    void update(size_t id, float value, uint8_t dec = 2) {
        p.beginObj();
        p.addUint(sets::Code::id, id);
        p.addFloat(sets::Code::value, value, dec);
        p.endObj();
    }

    void update(size_t id, double value, uint8_t dec = 2) {
        update(id, (float)value, dec);
    }

    void update(size_t id, short value) {
        _updateInt(id, value);
    }
    void update(size_t id, int value) {
        _updateInt(id, value);
    }
    void update(size_t id, long value) {
        _updateInt(id, value);
    }

    void update(size_t id, unsigned short value) {
        _updateUint(id, value);
    }
    void update(size_t id, unsigned int value) {
        _updateUint(id, value);
    }
    void update(size_t id, unsigned long value) {
        _updateUint(id, value);
    }

   private:
    Packet& p;

    void _updateInt(size_t id, int32_t value) {
        p.beginObj();
        p.addUint(sets::Code::id, id);
        p.addInt(sets::Code::value, value);
        p.endObj();
    }

    void _updateUint(size_t id, uint32_t value) {
        p.beginObj();
        p.addUint(sets::Code::id, id);
        p.addUint(sets::Code::value, value);
        p.endObj();
    }
};

}  // namespace sets