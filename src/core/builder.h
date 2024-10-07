#pragma once
#include <Arduino.h>

#include "AnyPtr.h"
#include "build.h"
#include "colors.h"
#include "containers_class.h"
#include "packet.h"

#define _NO_ID (size_t) - 1

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

    // ================= LABEL =================
    // текстовое значение, может обновляться по id
    void Label(size_t id, Text label = "", Text text = Text(), uint32_t color = SETS_DEFAULT_COLOR) {
        _widget(Code::label, id, label, &text, color);
    }
    void Label(size_t id, Text label, Text text, sets::Colors color) {
        Label(id, label, text, (uint32_t)color);
    }
    void Label(Text label = "", Text text = Text(), uint32_t color = SETS_DEFAULT_COLOR) {
        Label(_NO_ID, label, text, color);
    }
    void Label(Text label, Text text, sets::Colors color) {
        Label(_NO_ID, label, text, (uint32_t)color);
    }

    // ================= LED =================
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
    bool Switch(size_t id, Text label = "", bool* value = nullptr) {
        _widget(Code::toggle, id, label, value);
        return _isSet(id, value);
    }
    bool Switch(Text label = "", bool* value = nullptr) {
        return Switch(_next(), label, value);
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
    bool Slider(size_t id, Text label = "", float min = 0, float max = 100, float step = 1, Text unit = Text(), AnyPtr value = nullptr) {
        if (_beginWidget(Code::slider, id, label, value)) {
            p->addFloat(Code::min, min);
            p->addFloat(Code::max, max);
            p->addFloat(Code::step, step);
            if (unit.length()) p->addText(Code::unit, unit);
            _endWidget();
        }
        return _isSet(id, value);
    }
    bool Slider(Text label = "", float min = 0, float max = 100, float step = 1, Text unit = Text(), AnyPtr value = nullptr) {
        return Slider(_next(), label, min, max, step, unit, value);
    }

    // ================= SELECT =================
    // опции разделяются ; [результат - индекс (число)], подключаемая переменная - uint8_t
    bool Select(size_t id, Text label, Text options, uint8_t* value = nullptr) {
        if (_beginWidget(Code::select, id, label, value)) {
            p->addText(Code::text, options);
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

    bool Button(size_t id, Text label, sets::Colors color) {
        return Button(id, label, (uint32_t)color);
    }
    bool Button(Text label, sets::Colors color) {
        return Button(_next(), label, (uint32_t)color);
    }

    // misc
    // окно подтверждения, для активации отправь пустой update на его id или update с текстом подтверждения
    bool Confirm(size_t id, Text label = "") {
        _widget(Code::confirm, id, label);
        return _isSet(id, nullptr);
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
            p->beginObj();
            p->addCode(Code::type, type);
            if (label.length()) p->addText(Code::label, label);
            if (color != SETS_DEFAULT_COLOR) p->addUint(Code::color, color);
            if (id != _NO_ID) p->addUint(Code::id, id);

            if (value) {
                p->addKey(Code::value);
                value.write(p);
            } else if (_db && id != _NO_ID) {
                p->addKey(Code::value);
                p->addFromDB(_db, id);
            }
            return true;
        }
        return false;
    }

    void _endWidget() {
        p->endObj();
    }

    bool _beginContainer(Code type, Text title = Text()) {
        if (build.isBuild()) {
            p->beginObj();
            p->addCode(Code::type, type);
            if (title.length()) p->addText(Code::title, title);
            p->beginArr(Code::content);
        }
        return true;
    }

    void _endContainer() {
        if (build.isBuild()) {
            p->endArr();
            p->endObj();
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