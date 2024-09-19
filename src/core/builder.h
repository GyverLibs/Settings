#pragma once
#include <Arduino.h>
#include <GyverDB.h>
#include <StreamWriter.h>

#include "build.h"
#include "colors.h"
#include "containers_class.h"
#include "packet.h"

namespace sets {

class Builder {
    friend class BasicContainer;

   public:
    Builder(Build build, GyverDB* db = nullptr, sets::Packet* p = nullptr) : _build(build), _db(db), p(p) {
        endGuest();
    }

    // инфо о билде
    Build build() {
        return _build;
    }

    // перезагрузить страницу
    void reload() {
        if (_build.isAction()) _reload = 1;
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
        if (!_build.isGranted()) _enabled = true;
        return true;
    }

    // запретить неавторизованным клиентам
    void endGuest() {
        if (!_build.isGranted()) _enabled = false;
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
    void Label(size_t id, Text label, Text text = Text(), uint32_t color = SETS_DEFAULT_COLOR) {
        if (_isBuild()) {
            _beginWidget(Code::label, id, label, text);
            if (color != SETS_DEFAULT_COLOR) p->addUint(Code::color, color);
            _endWidget();
        }
    }
    void Label(size_t id, Text label, Text text, sets::Colors color) {
        Label(id, label, text, (uint32_t)color);
    }
    void Label(Text label, Text text = Text(), uint32_t color = SETS_DEFAULT_COLOR) {
        Label(-1, label, text, color);
    }
    void Label(Text label, Text text, sets::Colors color) {
        Label(label, text, (uint32_t)color);
    }

    // ================= LED =================
    // светодиод (value true включен - зелёный, value false выключен - красный)
    void LED(size_t id, Text label, bool value) {
        if (_isBuild()) {
            _beginWidget(Code::led, id, label, Value(value));
            _endWidget();
        }
    }
    void LED(Text label, bool value) {
        LED(-1, label, value);
    }

    // ================= TEXT =================
    // текстовый абзац
    void Paragraph(size_t id, Text label, Text text = Text()) {
        if (_isBuild()) _widget(Code::paragraph, id, label, text);
    }

    // active

    // ================= INPUT =================
    // ввод текста и цифр [результат - строка]
    bool Input(size_t id, Text label, Text value = Text()) {
        if (_isBuild()) _widget(Code::input, id, label, value);
        return _isSet(id);
    }

    // ================= PASS =================
    // ввод пароля [результат - строка]
    bool Pass(size_t id, Text label, Text value = Text()) {
        if (_isBuild()) _widget(Code::pass, id, label, value);
        return _isSet(id);
    }

    // ================= COLOR =================
    // ввод цвета [результат - 24-бит DEC число]
    bool Color(size_t id, Text label, Text value = Text()) {
        if (_isBuild()) _widget(Code::color, id, label, value);
        return _isSet(id);
    }

    // ================= SWITCH =================
    // переключатель [результат 1/0]
    bool Switch(size_t id, Text label, Text value = Text()) {
        if (_isBuild()) _widget(Code::toggle, id, label, value);
        return _isSet(id);
    }

    // ================= SWITCH =================
    // дата [результат - unix секунды]
    bool Date(size_t id, Text label, Text value = Text()) {
        if (_isBuild()) _widget(Code::date, id, label, value);
        return _isSet(id);
    }

    // ================= SWITCH =================
    // время [результат - секунды с начала суток]
    bool Time(size_t id, Text label, Text value = Text()) {
        if (_isBuild()) _widget(Code::time, id, label, value);
        return _isSet(id);
    }

    // ================= SWITCH =================
    // дата и время [результат - unix секунды]
    bool DateTime(size_t id, Text label, Text value = Text()) {
        if (_isBuild()) _widget(Code::datetime, id, label, value);
        return _isSet(id);
    }

    // ================= SLIDER =================
    // слайдер [результат - число]
    bool Slider(size_t id, Text label, float min = 0, float max = 100, float step = 1, Text unit = Text(), Text value = Text()) {
        if (_isBuild()) {
            _beginWidget(Code::slider, id, label, value);
            p->addFloat(Code::min, min);
            p->addFloat(Code::max, max);
            p->addFloat(Code::step, step);
            p->addText(Code::unit, unit);
            _endWidget();
        }
        return _isSet(id);
    }

    // ================= SELECT =================
    // опции разделяются ; [результат - индекс (число)]
    bool Select(size_t id, Text label, Text options, Text value = Text()) {
        if (_isBuild()) {
            _beginWidget(Code::select, id, label, value);
            p->addText(Code::text, options);
            _endWidget();
        }
        return _isSet(id);
    }

    // ================= BUTTON =================
    // кнопку можно добавлять как внутри контейнера кнопок, так и как одиночный виджет
    bool Button(size_t id, Text label, uint32_t color = SETS_DEFAULT_COLOR) {
        if (_isBuild()) {
            _beginWidget(Code::button, id, label);
            if (color != SETS_DEFAULT_COLOR) p->addUint(Code::color, color);
            _endWidget();
        }
        return _isSet(id);
    }
    bool Button(size_t id, Text label, sets::Colors color) {
        return Button(id, label, (uint32_t)color);
    }

    // misc
    // окно подтверждения, для активации отправь пустой update на его id или update с текстом подтверждения
    bool Confirm(size_t id, Text label = "") {
        if (_isBuild()) _widget(Code::confirm, id, label);
        return _isSet(id);
    }

   private:
    Build _build;
    GyverDB* _db = nullptr;
    sets::Packet* p = nullptr;
    bool _reload = false;
    bool _enabled = true;
    bool _was_set = false;

    void _widget(Code type, size_t id, Text& label, const Text& value = Text()) {
        _beginWidget(type, id, label, value);
        _endWidget();
    }

    void _beginWidget(Code type, size_t id, Text& label, const Text& value = Text()) {
        p->beginObj();
        p->addCode(Code::type, type);
        if (id != (size_t)-1) p->addUint(Code::id, id);
        p->addText(Code::label, label);
        if (value) p->addText(Code::value, value);
        else if (_db) {
            p->addKey(Code::value);
            p->addFromDB(_db, id);
        }
    }

    void _endWidget() {
        p->endObj();
    }

    bool _beginContainer(Code type, Text title = Text()) {
        if (_build.isBuild()) {
            p->beginObj();
            p->addCode(Code::type, type);
            if (title) p->addText(Code::title, title);
            p->beginArr(Code::content);
        }
        return true;
    }

    void _endContainer() {
        if (_build.isBuild()) {
            p->endArr();
            p->endObj();
        }
    }

    bool _isBuild() {
        return _enabled && _build.isBuild();
    }

    bool _isSet(size_t id) {
        bool set = (_enabled && _build.isAction() && id == _build.id());
        _was_set |= set;
        return set;
    }
};

}  // namespace sets