#pragma once
#include <Arduino.h>

#include "AnyPtr.h"
#include "build.h"
#include "colors.h"
#include "containers_class.h"
#include "logger.h"
#include "packet.h"

#define _NO_ID ((size_t)(-1))

namespace sets {

class Builder {
    friend class BasicContainer;

   public:
    Builder(void* settings, Build& build, sets::Packet* p = nullptr, void* db = nullptr) : build(build), _settings(settings), p(p), _db(db) {
        endGuest();
    }

    // инфо о билде
    Build& build;

    // авто-ID следующего виджета
    size_t nextID() {
        return _auto_id - 1;
    }

    // указатель на текущий SettingsXxx
    void* thisSettings() {
        return _settings;
    }

    // перезагрузить страницу
    void reload() {
        if (build.isAction()) _reload = 1;
    }

    // страница будет перезагружена
    bool isReload() {
        return _reload;
    }

    // было действие с каким-то из виджетов выше
    bool wasSet() {
        return _was_set;
    }

    // сбросить флаг чтения wasSet
    void clearSet() {
        _was_set = false;
    }

    // ================= GUEST =================

    // разрешить неавторизованным клиентам следующий код
    bool beginGuest() {
        if (!build.granted) _enabled = true;
        return true;
    }

    // запретить неавторизованным клиентам
    void endGuest() {
        if (!build.granted) _enabled = false;
    }

    // ================= GROUP =================

    // группа виджетов
    bool beginGroup(Text title = Text()) {
        return _beginContainer(Code::group, title);
    }
    void endGroup() {
        _endContainer();
    }

    // ================= ROW =================

    // горизонтальная группа виджетов
    bool beginRow(Text title = Text()) {
        return _beginContainer(Code::row, title);
    }
    void endRow() {
        _endContainer();
    }

    // ================= MENU =================

    // вложенное меню
    bool beginMenu(Text title) {
        return _beginContainer(Code::menu, title);
    }
    void endMenu() {
        _endContainer();
    }

    // ================= BUTTONS =================

    // ряд кнопок
    bool beginButtons() {
        return _beginContainer(Code::buttons);
    }
    void endButtons() {
        _endContainer();
    }

    // passive
    // ================= LOG =================
    void Log(size_t id, Logger& log, Text label = "") {
        if (_enabled && build.isBuild()) {
            (*p)('{');
            (*p)[Code::type] = Code::log;
            if (label.length()) (*p)[Code::label] = label;
            if (id != _NO_ID) (*p)[Code::id] = id;
            (*p)[Code::value];
            p->addLogger(log);
            (*p)('}');
            p->checkLen();
        }
    }
    void Log(Logger& log, Text label = "") {
        Log(_NO_ID, log, label);
    }

    // ================= LABEL =================
    // текстовое значение, может обновляться по id
    void Label(size_t id, Text label = "", Text text = Text(), uint32_t color = SETS_DEFAULT_COLOR) {
        _widget(Code::label, id, label, &text, color);
    }
    void Label(size_t id, Text label, Text text, Colors color) {
        Label(id, label, text, (uint32_t)color);
    }
    void Label(Text label = "", Text text = Text(), uint32_t color = SETS_DEFAULT_COLOR) {
        Label(_NO_ID, label, text, color);
    }
    void Label(Text label, Text text, Colors color) {
        Label(_NO_ID, label, text, (uint32_t)color);
    }

    // лейбл с численным значением (выполняется быстрее, весит меньше)
    template <typename T>
    void LabelNum(size_t id, Text label, T text, uint32_t color = SETS_DEFAULT_COLOR) {
        _widget(Code::label, id, label, &text, color);
    }
    template <typename T>
    void LabelNum(size_t id, Text label, T text, Colors color) {
        LabelNum(id, label, text, (uint32_t)color);
    }
    template <typename T>
    void LabelNum(Text label, T text, uint32_t color = SETS_DEFAULT_COLOR) {
        LabelNum(_NO_ID, label, text, color);
    }
    template <typename T>
    void LabelNum(Text label, T text, Colors color) {
        LabelNum(_NO_ID, label, text, (uint32_t)color);
    }

    // лейбл с численным значением (выполняется быстрее, весит меньше)
    void LabelFloat(size_t id, Text label, float text, uint8_t dec = 2, uint32_t color = SETS_DEFAULT_COLOR) {
        if (_beginWidget(Code::label, id, label, &text, color)) {
            (*p)[Code::step] = dec;
            _endWidget();
        }
    }
    void LabelFloat(size_t id, Text label, float text, uint8_t dec, Colors color) {
        LabelFloat(id, label, text, dec, (uint32_t)color);
    }
    void LabelFloat(Text label, float text, uint8_t dec = 2, uint32_t color = SETS_DEFAULT_COLOR) {
        LabelFloat(_NO_ID, label, text, dec, color);
    }
    void LabelFloat(Text label, float text, uint8_t dec, Colors color) {
        LabelFloat(_NO_ID, label, text, dec, (uint32_t)color);
    }

    // ================= LED =================
    // светодиод с цветом на выбор
    void LED(size_t id, Text label, bool value, uint32_t colorOff, uint32_t colorOn) {
        if (_beginWidget(Code::led, id, label, &value)) {
            (*p)[Code::color_off] = colorOff;
            (*p)[Code::color_on] = colorOn;
            _endWidget();
        }
    }
    void LED(size_t id, Text label, bool value, Colors colorOff, Colors colorOn) {
        LED(id, label, value, (uint32_t)colorOff, (uint32_t)colorOn);
    }
    void LED(Text label, bool value, uint32_t colorOff, uint32_t colorOn) {
        LED(_NO_ID, label, value, colorOff, colorOn);
    }
    void LED(Text label, bool value, Colors colorOff, Colors colorOn) {
        LED(_NO_ID, label, value, (uint32_t)colorOff, (uint32_t)colorOn);
    }

    // светодиод (value 1 включен - зелёный, value 0 выключен - красный)
    void LED(size_t id, Text label, bool value) {
        _widget(Code::led, id, label, &value);
    }
    void LED(size_t id, Text label = "") {
        _widget(Code::led, id, label);
    }
    void LED(Text label, bool value) {
        LED(_NO_ID, label, value);
    }
    void LED(Text label = "") {
        LED(_NO_ID, label);
    }

    // ================= TEXT =================
    // текстовый абзац
    void Paragraph(size_t id, Text label = "", Text text = Text()) {
        _widget(Code::paragraph, id, label, &text);
    }
    void Paragraph(Text label = "", Text text = Text()) {
        Paragraph(_NO_ID, label, text);
    }

    // active

    // ================= INPUT =================
    // ввод текста и цифр [результат - строка], подключаемая переменная - любой тип
    bool Input(size_t id, Text label = "", AnyPtr value = nullptr) {
        _widget(Code::input, id, label, value);
        return _isSet(id, value);
    }
    bool Input(Text label = "", AnyPtr value = nullptr) {
        return Input(_next(), label, value);
    }

    // ================= NUMBER =================
    // ввод цифр [результат - строка], подключаемая переменная - любой тип
    bool Number(size_t id, Text label = "", AnyPtr value = nullptr) {
        _widget(Code::number, id, label, value);
        return _isSet(id, value);
    }
    bool Number(Text label = "", AnyPtr value = nullptr) {
        return Number(_next(), label, value);
    }

    // ================= PASS =================
    // ввод пароля [результат - строка], подключаемая переменная - любой тип
    bool Pass(size_t id, Text label = "", AnyPtr value = nullptr) {
        _widget(Code::pass, id, label, value);
        return _isSet(id, value);
    }
    bool Pass(Text label = "", AnyPtr value = nullptr) {
        return Pass(_next(), label, value);
    }

    // ================= COLOR =================
    // ввод цвета [результат - 24-бит DEC число], подключаемая переменная - uint32_t
    bool Color(size_t id, Text label = "", uint32_t* value = nullptr) {
        _widget(Code::color, id, label, value);
        return _isSet(id, value);
    }
    bool Color(Text label = "", uint32_t* value = nullptr) {
        return Color(_next(), label, value);
    }

    // ================= SWITCH =================
    // переключатель [результат 1/0], подключаемая переменная - bool
    bool Switch(size_t id, Text label = "", bool* value = nullptr, uint32_t color = SETS_DEFAULT_COLOR) {
        _widget(Code::toggle, id, label, value, color);
        return _isSet(id, value);
    }
    bool Switch(size_t id, Text label, bool* value, Colors color) {
        return Switch(id, label, value, (uint32_t)color);
    }
    bool Switch(Text label = "", bool* value = nullptr, uint32_t color = SETS_DEFAULT_COLOR) {
        return Switch(_next(), label, value, color);
    }
    bool Switch(Text label, bool* value, Colors color) {
        return Switch(_next(), label, value, (uint32_t)color);
    }

    // ================= DATE =================
    // дата [результат - unix секунды], подключаемая переменная - uint32_t
    bool Date(size_t id, Text label = "", uint32_t* value = nullptr) {
        _widget(Code::date, id, label, value);
        return _isSet(id, value);
    }
    bool Date(Text label = "", uint32_t* value = nullptr) {
        return Date(_next(), label, value);
    }

    // ================= TIME =================
    // время [результат - секунды с начала суток], подключаемая переменная - uint32_t
    bool Time(size_t id, Text label = "", uint32_t* value = nullptr) {
        _widget(Code::time, id, label, value);
        return _isSet(id, value);
    }
    bool Time(Text label = "", uint32_t* value = nullptr) {
        return Time(_next(), label, value);
    }

    // ================= DATETIME =================
    // дата и время [результат - unix секунды], подключаемая переменная - uint32_t
    bool DateTime(size_t id, Text label = "", uint32_t* value = nullptr) {
        _widget(Code::datetime, id, label, value);
        return _isSet(id, value);
    }
    bool DateTime(Text label = "", uint32_t* value = nullptr) {
        return DateTime(_next(), label, value);
    }

    // ================= SLIDER =================
    // слайдер [результат - число], подключаемая переменная - любой тип
    bool Slider(size_t id, Text label = "", float min = 0, float max = 100, float step = 1, Text unit = Text(), AnyPtr value = nullptr, uint32_t color = SETS_DEFAULT_COLOR) {
        if (_beginWidget(Code::slider, id, label, value, color)) {
            (*p)[Code::min] = min;
            (*p)[Code::max] = max;
            (*p)[Code::step] = step;
            if (unit.length()) (*p)[Code::unit] = unit;
            _endWidget();
        }
        return _isSet(id, value);
    }
    bool Slider(size_t id, Text label, float min, float max, float step, Text unit, AnyPtr value, Colors color) {
        return Slider(id, label, min, max, step, unit, value, (uint32_t)color);
    }
    bool Slider(Text label = "", float min = 0, float max = 100, float step = 1, Text unit = Text(), AnyPtr value = nullptr, uint32_t color = SETS_DEFAULT_COLOR) {
        return Slider(_next(), label, min, max, step, unit, value, color);
    }
    bool Slider(Text label, float min, float max, float step, Text unit, AnyPtr value, Colors color) {
        return Slider(_next(), label, min, max, step, unit, value, (uint32_t)color);
    }

    // ================= SLIDER2 =================
    // двойной слайдер [результат - число], подключаемая переменная - любой тип
    bool Slider2(size_t id_min, size_t id_max, Text label = "", float min = 0, float max = 100, float step = 1, Text unit = Text(), AnyPtr value_min = nullptr, AnyPtr value_max = nullptr, uint32_t color = SETS_DEFAULT_COLOR) {
        if (_enabled && build.isBuild()) {
            (*p)('{');
            (*p)[Code::type] = Code::slider2;
            if (label.length()) (*p)[Code::label] = label;
            if (color != SETS_DEFAULT_COLOR) (*p)[Code::color] = color;
            (*p)[Code::min] = min;
            (*p)[Code::max] = max;
            (*p)[Code::step] = step;
            if (unit.length()) (*p)[Code::unit] = unit;
            (*p)[Code::id] = id_min;
            (*p)[Code::id2] = id_max;

            if ((*p)[Code::value]('[')) {
                if (value_min) value_min.write(p);
                else if (_db) p->addFromDB(_db, id_min);

                if (value_max) value_max.write(p);
                else if (_db) p->addFromDB(_db, id_max);
                (*p)(']');
            }

            (*p)('}');
            p->checkLen();
        }
        bool set = _isSet(id_min, value_min);
        set |= _isSet(id_max, value_max);
        return set;
    }
    bool Slider2(size_t id_min, size_t id_max, Text label, float min, float max, float step, Text unit, AnyPtr value_min, AnyPtr value_max, Colors color) {
        return Slider2(id_min, id_max, label, min, max, step, unit, value_min, value_max, (uint32_t)color);
    }

    // ================= SELECT =================
    // опции разделяются ; [результат - индекс (число)], подключаемая переменная - uint8_t
    bool Select(size_t id, Text label, Text options, uint8_t* value = nullptr) {
        if (_beginWidget(Code::select, id, label, value)) {
            (*p)[Code::text] = options;
            _endWidget();
        }
        return _isSet(id, value);
    }
    bool Select(Text label, Text options, uint8_t* value = nullptr) {
        return Select(_next(), label, options, value);
    }

    // ================= BUTTON =================
    // кнопку можно добавлять как внутри контейнера кнопок, так и как одиночный виджет
    bool Button(size_t id, Text label = "", uint32_t color = SETS_DEFAULT_COLOR) {
        _widget(Code::button, id, label, nullptr, color);
        return _isSet(id, nullptr);
    }
    bool Button(Text label = "", uint32_t color = SETS_DEFAULT_COLOR) {
        return Button(_next(), label, color);
    }

    bool Button(size_t id, Text label, Colors color) {
        return Button(id, label, (uint32_t)color);
    }
    bool Button(Text label, Colors color) {
        return Button(_next(), label, (uint32_t)color);
    }

    // misc
    // окно подтверждения, для активации отправь пустой update на его id или update с текстом подтверждения
    bool Confirm(size_t id, Text label = "", bool* ptr = nullptr) {
        _widget(Code::confirm, id, label);
        return _isSet(id, ptr);
    }

    // ================= CUSTOM =================
    // кастомный виджет, type соответствует имени класса. params - ключи и значения
    bool Custom(Text type, size_t id, const BSON& params = BSON(), AnyPtr value = nullptr) {
        if (_enabled && build.isBuild()) {
            (*p)('{');
            (*p)[Code::id] = id;
            (*p)[Code::type] = type;
            p->add(params);

            if (value) {
                (*p)[Code::value];
                value.write(p);
            } else if (_db) {
                (*p)[Code::value];
                p->addFromDB(_db, id);
            }

            (*p)('}');
            p->checkLen();
            return _isSet(id, value);
        }
        return false;
    }

   private:
    void* _settings;
    sets::Packet* p = nullptr;
    void* _db = nullptr;
    size_t _auto_id = UINT32_MAX;
    bool _reload = false;
    bool _enabled = true;
    bool _was_set = false;
    bool _set_f = false;

    size_t _next() {
        return --_auto_id;
    }

    bool _widget(Code type, size_t id, Text& label, AnyPtr value = nullptr, uint32_t color = SETS_DEFAULT_COLOR) {
        if (_beginWidget(type, id, label, value, color)) {
            _endWidget();
            return true;
        }
        return false;
    }

    bool _beginWidget(Code type, size_t id, Text& label, AnyPtr value = nullptr, uint32_t color = SETS_DEFAULT_COLOR) {
        if (_enabled && build.isBuild()) {
            (*p)('{');
            (*p)[Code::type] = type;
            if (label.length()) (*p)[Code::label] = label;
            if (color != SETS_DEFAULT_COLOR) (*p)[Code::color] = color;
            if (id != _NO_ID) (*p)[Code::id] = id;

            if (value) {
                (*p)[Code::value];
                value.write(p);
            } else if (_db && id != _NO_ID) {
                (*p)[Code::value];
                p->addFromDB(_db, id);
            }
            return true;
        }
        return false;
    }

    void _endWidget() {
        (*p)('}');
        p->checkLen();
    }

    bool _beginContainer(Code type, Text title = Text()) {
        if (build.isBuild()) {
            (*p)('{');
            (*p)[Code::type] = type;
            if (title.length()) (*p)[Code::title] = title;
            (*p)[Code::content]('[');
        }
        return true;
    }

    void _endContainer() {
        if (build.isBuild()) {
            (*p)(']');
            (*p)('}');
        }
    }

    bool _isSet(size_t id, AnyPtr value) {
        bool set = (!_set_f && _enabled && build.isAction() && id == build.id);
        if (set) _set_f = true;
        if (value && set) value.read(build.value);
        _was_set |= set;
        return set;
    }
};

}  // namespace sets