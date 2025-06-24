#pragma once
#include <Arduino.h>

#ifndef SETT_NO_TABLE
#include <Table.h>
#endif

#include "colors.h"
#include "logger.h"
#include "packet.h"
#include "tmode.h"

namespace sets {

class Updater {
   public:
    Updater(Packet& p) : p(p) {}

    // всплывающее уведомление красное
    Updater& alert(Text text) {
        p('{');
        p[Code::type] = Code::alert;
        p[Code::text] = text;
        p('}');
        return *this;
    }

    // всплывающее уведомление зелёное
    Updater& notice(Text text) {
        p('{');
        p[Code::type] = Code::notice;
        p[Code::text] = text;
        p('}');
        return *this;
    }

    // вызов виджета Confirm
    Updater& confirm(size_t id) {
        return update(id, true);
    }

    // апдейт логгера
    Updater& update(size_t id, Logger& logger) {
        if (logger._changed()) {
            p('{');
            p[Code::id] = id;
            p[Code::data];
            p.addLogger(logger);
            p('}');
            logger._reset();
        }
        return *this;
    }

    // пустой апдейт
    Updater& update(size_t id) {
        return update(id, true);
    }

    // апдейт с цветом
    Updater& updateColor(size_t id, uint32_t color) {
        p('{');
        p[Code::id] = id;
        p[Code::color] = color;
        p('}');
        return *this;
    }
    Updater& updateColor(size_t id, sets::Colors color) {
        return updateColor(id, (uint32_t)color);
    }

    // апдейт с числом
    template <typename T>
    Updater& update(size_t id, T value) {
        p('{');
        p[Code::id] = id;
        p[Code::data] = value;
        p('}');
        return *this;
    }

    // апдейт с float
    Updater& update(size_t id, float value, int dec = 2) {
        p('{');
        p[Code::id] = id;
        p[Code::data].add(value, dec);
        p('}');
        return *this;
    }
    Updater& update(size_t id, double value, int dec = 2) {
        return update(id, (float)value, dec);
    }

    // апдейт с текстом
    Updater& updateText(size_t id, const Text& value) {
        p('{');
        p[Code::id] = id;
        p[Code::data] = value;
        p('}');
        return *this;
    }
    Updater& update(size_t id, const Text& value) {
        return updateText(id, value);
    }
    Updater& update(size_t id, const String& value) {
        return updateText(id, value);
    }
    Updater& update(size_t id, const char* value) {
        return updateText(id, value);
    }
    Updater& update(size_t id, char* value) {
        return updateText(id, value);
    }
    Updater& update(size_t id, const __FlashStringHelper* value) {
        return updateText(id, value);
    }

    // апдейт для двойного слайдера
    template <typename T>
    Updater& update2(size_t id_min, T value_min, T value_max) {
        p('{');
        p[Code::id] = id_min;
        if (p[Code::data]('[')) {
            p += value_min;
            p += value_max;
            p(']');
        }
        p('}');
        return *this;
    }
    Updater& update2(size_t id_min, float value_min, float value_max, int dec = 2) {
        p('{');
        p[Code::id] = id_min;
        if (p[Code::data]('[')) {
            p.add(value_min, dec);
            p.add(value_max, dec);
            p(']');
        }
        p('}');
        return *this;
    }

    // кастом апдейт для кастом виджета, params - ключи и значения
    Updater& update(size_t id, BSON& params) {
        p('{');
        p[Code::id] = id;

        p[Code::data]('{');
        p.add(params);
        p('}');

        p('}');
        return *this;
    }

#ifndef SETT_NO_TABLE
    // апдейт для Table
    Updater& updateTable(size_t id, Table& table) {
        if (table.rows()) {
            p('{');
            p[Code::id] = id;
            p[Code::data];
            p.beginBin(table.writeSize());
            table.writeTo(p);
            p('}');
        }
        return *this;
    }
#endif

    // апдейт для running и stack графиков
    template <typename T>
    Updater& updatePlot(size_t id, const T& data) {
        p('{');
        p[Code::id] = id;
        p[Code::data];
        p.add(&data, sizeof(T));
        p('}');
        return *this;
    }

    // апдейт для графиков из файла
    Updater& updatePlot(size_t id, char* path) {
        return update(id, path);
    }
    Updater& updatePlot(size_t id, const char* path) {
        return update(id, path);
    }
    Updater& updatePlot(size_t id, const __FlashStringHelper* path) {
        return update(id, path);
    }
    Updater& updatePlot(size_t id, const String& path) {
        return update(id, path);
    }
    Updater& updatePlot(size_t id, const Text& path) {
        return update(id, path);
    }

#ifndef SETT_NO_TABLE
    // апдейт для plot графиков
    Updater& updatePlot(size_t id, Table& table, bool clearTable = true) {
        if (table.rows()) {
            p('{');
            p[Code::id] = id;
            p[Code::data];
            p.beginBin(table.writeSize());
            table.writeTo(p);
            if (clearTable) table.removeAll();
            p('}');
        }
        return *this;
    }

    // апдейт для timeline графиков
    Updater& updatePlot(size_t id, Table& table, TMode mode, bool clearTable = true) {
        if (table.rows()) {
            p('{');
            p[Code::id] = id;
            p[Code::tmode] = (uint8_t)mode;
            p[Code::data];
            p.beginBin(table.writeSize());
            table.writeTo(p);
            if (clearTable) table.removeAll();
            p('}');
        }
        return *this;
    }
#endif

   private:
    Packet& p;
};

}  // namespace sets