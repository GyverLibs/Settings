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
    Builder(Build build, GyverDB* db = nullptr, sets::Packet* p = nullptr) : _build(build), _db(db), p(p) {}

    // инфо о билде
    Build build() {
        return _build;
    }

    // перезагрузить страницу
    void reload() {
        if (_build.isAction()) _reload = 1;
    }
    bool isReload() {
        return _reload;
    }

    // ================= GROUP =================
    bool beginGroup(Text title = Text()) {
        return _beginContainer(Code::group, title);
    }
    void endGroup() {
        _endContainer();
    }

    // ================= MENU =================
    bool beginMenu(Text title) {
        return _beginContainer(Code::menu, title);
    }
    void endMenu() {
        _endContainer();
    }

    // ================= BUTTONS =================
    bool beginButtons() {
        return _beginContainer(Code::buttons);
    }
    void endButtons() {
        _endContainer();
    }

    // passive

    // ================= LABEL =================
    void Label(size_t id, Text label, Text text = Text(), uint32_t color = SETS_DEFAULT_COLOR) {
        if (_build.isLoad()) {
            _beginWidget(Code::label, id, label, text);
            if (color != SETS_DEFAULT_COLOR) p->addUint(Code::color, color);
            _endWidget();
        }
    }
    void Label(size_t id, Text label, Text text, sets::Colors color) {
        Label(id, label, text, (uint32_t)color);
    }

    // ================= TEXT =================
    void Paragraph(size_t id, Text label, Text text = Text()) {
        if (_build.isLoad()) _widget(Code::paragraph, id, label, text);
    }

    // active

    // ================= INPUT =================
    bool Input(size_t id, Text label, Text value = Text()) {
        if (_build.isLoad()) _widget(Code::input, id, label, value);
        return _checkSet(id);
    }

    // ================= PASS =================
    bool Pass(size_t id, Text label, Text value = Text()) {
        if (_build.isLoad()) _widget(Code::pass, id, label, value);
        return _checkSet(id);
    }

    // ================= COLOR =================
    bool Color(size_t id, Text label, Text value = Text()) {
        if (_build.isLoad()) _widget(Code::color, id, label, value);
        return _checkSet(id);
    }

    // ================= SWITCH =================
    bool Switch(size_t id, Text label, Text value = Text()) {
        if (_build.isLoad()) _widget(Code::toggle, id, label, value);
        return _checkSet(id);
    }

    // ================= SWITCH =================
    bool Date(size_t id, Text label, Text value = Text()) {
        if (_build.isLoad()) _widget(Code::date, id, label, value);
        return _checkSet(id);
    }

    // ================= SWITCH =================
    bool Time(size_t id, Text label, Text value = Text()) {
        if (_build.isLoad()) _widget(Code::time, id, label, value);
        return _checkSet(id);
    }

    // ================= SWITCH =================
    bool DateTime(size_t id, Text label, Text value = Text()) {
        if (_build.isLoad()) _widget(Code::datetime, id, label, value);
        return _checkSet(id);
    }

    // ================= SLIDER =================
    bool Slider(size_t id, Text label, float min = 0, float max = 100, float step = 1, Text unit = Text(), Text value = Text()) {
        if (_build.isLoad()) {
            _beginWidget(Code::slider, id, label, value);
            p->addFloat(Code::min, min);
            p->addFloat(Code::max, max);
            p->addFloat(Code::step, step);
            p->addText(Code::unit, unit);
            _endWidget();
        }
        return _checkSet(id);
    }

    // ================= SELECT =================
    // опции разделяются ;
    bool Select(size_t id, Text label, Text options, Text value = Text()) {
        if (_build.isLoad()) {
            _beginWidget(Code::select, id, label, value);
            p->addText(Code::text, options);
            _endWidget();
        }
        return _checkSet(id);
    }

    // ================= BUTTON =================
    bool Button(size_t id, Text label, uint32_t color = SETS_DEFAULT_COLOR) {
        if (_build.isLoad()) {
            _beginWidget(Code::button, id, label);
            if (color != SETS_DEFAULT_COLOR) p->addUint(Code::color, color);
            _endWidget();
        }
        return _checkSet(id);
    }
    bool Button(size_t id, Text label, sets::Colors color) {
        return Button(id, label, (uint32_t)color);
    }

   private:
    Build _build;
    GyverDB* _db = nullptr;
    sets::Packet* p = nullptr;
    bool _reload = false;

    void _widget(Code type, size_t id, Text& label, const Text& value = Text()) {
        _beginWidget(type, id, label, value);
        _endWidget();
    }

    void _beginWidget(Code type, size_t id, Text& label, const Text& value = Text()) {
        p->beginObj();
        p->addCode(Code::type, type);
        p->addUint(Code::id, id);
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
        if (_build.isLoad()) {
            p->beginObj();
            p->addCode(Code::type, type);
            if (title) p->addText(Code::title, title);
            p->beginArr(Code::content);
        }
        return true;
    }

    void _endContainer() {
        if (_build.isLoad()) {
            p->endArr();
            p->endObj();
        }
    }

    bool _checkSet(size_t id) {
        return (_build.isAction() && id == _build.id());
    }
};

}  // namespace sets