#pragma once
#include <Arduino.h>

#include "colors.h"
#include "logger.h"
#include "packet.h"

namespace sets {

class Updater {
   public:
    Updater(Packet& p) : p(p) {}

    // всплывающее уведомление красное
    void alert(Text text) {
        p('{');
        p[Code::type] = Code::alert;
        p[Code::text] = text;
        p('}');
    }

    // всплывающее уведомление зелёное
    void notice(Text text) {
        p('{');
        p[Code::type] = Code::notice;
        p[Code::text] = text;
        p('}');
    }

    // вызов виджета Confirm
    void confirm(size_t id) {
        update(id, true);
    }

    // апдейт логгера
    void update(size_t id, Logger& logger) {
        if (!logger._changed()) return;
        p('{');
        p[Code::id] = id;
        p[Code::data];
        p.addLogger(logger);
        p('}');
        logger._reset();
    }

    // пустой апдейт
    void update(size_t id) {
        update(id, true);
    }

    // апдейт с цветом
    void updateColor(size_t id, uint32_t color) {
        p('{');
        p[Code::id] = id;
        p[Code::color] = color;
        p('}');
    }
    void updateColor(size_t id, sets::Colors color) {
        updateColor(id, (uint32_t)color);
    }

    // апдейт с числом
    template <typename T>
    void update(size_t id, T value) {
        p('{');
        p[Code::id] = id;
        p[Code::data] = value;
        p('}');
    }

    // апдейт с float
    void update(size_t id, float value, int dec = 2) {
        p('{');
        p[Code::id] = id;
        p[Code::data].add(value, dec);
        p('}');
    }
    void update(size_t id, double value, int dec = 2) {
        update(id, (float)value, dec);
    }

    // апдейт с текстом
    void update(size_t id, const Text& value) {
        updateText(id, value);
    }
    void update(size_t id, const String& value) {
        updateText(id, value);
    }
    void update(size_t id, const char* value) {
        updateText(id, value);
    }
    void update(size_t id, const __FlashStringHelper* value) {
        updateText(id, value);
    }

    // апдейт для двойного слайдера
    template <typename T>
    void update2(size_t id_min, T value_min, T value_max) {
        p('{');
        p[Code::id] = id_min;

        if (p[Code::data]('[')) {
            p += value_min;
            p += value_max;
            p(']');
        }

        p('}');
    }
    void update2(size_t id_min, float value_min, float value_max, int dec = 2) {
        p('{');
        p[Code::id] = id_min;

        if (p[Code::data]('[')) {
            p.add(value_min, dec);
            p.add(value_max, dec);
            p(']');
        }

        p('}');
    }

    // кастом апдейт для кастом виджета, params - ключи и значения
    void update(size_t id, BSON& params) {
        p('{');
        p[Code::id] = id;

        p[Code::data]('{');
        p.add(params);
        p('}');

        p('}');
    }

   private:
    Packet& p;

    void updateText(size_t id, const Text& value) {
        p('{');
        p[Code::id] = id;
        p[Code::data] = value;
        p('}');
    }
};

}  // namespace sets