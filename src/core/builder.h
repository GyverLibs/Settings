#pragma once
#include <Arduino.h>

#ifndef SETT_NO_TABLE
#include <Table.h>
#endif

#include "AnyPtr.h"
#include "build.h"
#include "colors.h"
#include "containers_class.h"
#include "logger.h"
#include "packet.h"
#include "pos.h"
#include "tmode.h"

#define _NO_ID ((size_t)(-1))

namespace sets {

enum class DivType : uint8_t {
    Default,
    Line,
    Block,
};

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
    void reload(bool force = false) {
        if (build.isAction()) _reload = force ? -1 : 1;
    }

    // страница будет перезагружена
    int8_t isReload() {
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

    // горизонтальная группа виджетов [DivType::Line | DivType::Block]
    bool beginRow(Text title = Text(), DivType divtype = DivType::Default) {
        return _beginContainer(Code::row, title, divtype);
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

    // true если в вебморде открыто текущее меню
    bool enterMenu() {
        return build.type == Build::Type::Menu && build.id == _menuID;
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
    // ================= LINK =================
    void Link(Text label, Text url) {
        _widget(Code::link, _NO_ID, label, &url);
    }

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

    // ================= IMAGE =================
    // изображение, url или path на флешке
    void Image(size_t id, Text label, Text path) {
        _widget(Code::image, id, label, &path);
    }
    void Image(Text label, Text path) {
        Image(_NO_ID, label, path);
    }

    // ================= STREAM =================
    // видео с камеры ESP32-CAM
    void Stream() {
        if (_enabled && build.isBuild()) {
            (*p)('{');
            (*p)[Code::type] = Code::stream;
            _endWidget();
        }
    }

    // ================= TEXT =================
    // текстовый абзац
    void Paragraph(size_t id, Text label = "", Text text = Text()) {
        _widget(Code::paragraph, id, label, &text);
    }
    void Paragraph(Text label = "", Text text = Text()) {
        Paragraph(_NO_ID, label, text);
    }

    // ================= HTML =================
    // HTML
    void HTML(size_t id, Text label = "", Text html = Text()) {
        _widget(Code::html, id, label, &html);
    }
    void HTML(Text label = "", Text html = Text()) {
        HTML(_NO_ID, label, html);
    }

    // ================= TABLE =================
    // таблица. CSV текст, или путь к CSV файлу .csv, или путь к бинарной таблице .tbl. Подписи - список с разделением ';'
    void Table(size_t id, Text table, Text labels = Text()) {
        _widget(Code::table, id, labels, &table);
    }
    void Table(Text table, Text labels = Text()) {
        Table(_NO_ID, table, labels);
    }

#ifndef SETT_NO_TABLE
    // таблица из бинарной таблицы в RAM. Подписи - список с разделением ';'
    void Table(size_t id, ::Table& table, Text labels = Text()) {
        if (_beginWidget(Code::table, id, labels)) {
            (*p)[Code::value];
            p->beginBin(table.writeSize());
            table.writeTo(*p);
            _endWidget();
        }
    }
    void Table(::Table& table, Text labels = Text()) {
        Table(_NO_ID, table, labels);
    }
#endif

    // ================= GAUGE =================
    // линейная шкала с заполнением, обновляется через апдейт
    void LinearGauge(size_t id, Text label = "", float min = 0, float max = 100, Text unit = Text(), float value = NAN, uint32_t color = SETS_DEFAULT_COLOR) {
        if (_beginWidget(Code::gauge, id, label, isnan(value) ? nullptr : &value, color)) {
            (*p)[Code::min] = min;
            (*p)[Code::max] = max;
            (*p)[Code::unit] = unit;
            _endWidget();
        }
    }
    void LinearGauge(size_t id, Text label, float min, float max, Text unit, float value, Colors color) {
        LinearGauge(id, label, min, max, unit, value, uint32_t(color));
    }
    void LinearGauge(Text label = "", float min = 0, float max = 100, Text unit = Text(), float value = NAN, uint32_t color = SETS_DEFAULT_COLOR) {
        LinearGauge(_NO_ID, label, min, max, unit, value, color);
    }
    void LinearGauge(Text label, float min, float max, Text unit, float value, Colors color) {
        LinearGauge(_NO_ID, label, min, max, unit, value, uint32_t(color));
    }

    // ================= PLOT =================
    // бегущий график. Принимает обновления вида float[]. Подписи разделяются ;
    void PlotRunning(size_t id, Text labels = Text(), uint16_t period = 200) {
        if (_beginWidget(Code::plot_run, id, labels)) {
            (*p)[Code::period] = period;
            _endWidget();
        }
    }
    void PlotRunning(Text labels = Text(), uint16_t period = 200) {
        PlotRunning(_NO_ID, labels, period);
    }

    // собирающийся график. Принимает обновления вида float[]. Подписи разделяются ;
    void PlotStack(size_t id, Text labels = Text()) {
        _widget(Code::plot_stack, id, labels);
    }
    void PlotStack(Text labels = Text()) {
        PlotStack(_NO_ID, labels);
    }

    // график с временем точек. Требует таблицу формата [unix, y1, y2...]. Путь к таблице в FS (.tbl, .csv). Подписи разделяются ;
    void Plot(size_t id, Text path = Text(), Text labels = Text()) {
        _widget(Code::plot, id, labels, &path);
    }
    void Plot(Text path = Text(), Text labels = Text()) {
        Plot(_NO_ID, path, labels);
    }

#ifndef SETT_NO_TABLE
    // график с временем точек. Требует таблицу формата [unix, y1, y2...]. Подписи разделяются ;
    void Plot(size_t id, ::Table& table, Text labels = Text(), bool clearTable = true) {
        if (_beginWidget(Code::plot, id, labels)) {
            (*p)[Code::value];
            p->beginBin(table.writeSize());
            table.writeTo(*p);
            if (clearTable) table.removeAll();
            _endWidget();
        }
    }
    void Plot(::Table& table, Text labels = Text(), bool clearTable = true) {
        Plot(_NO_ID, table, labels, clearTable);
    }
#endif

    // таймлайн. Требует таблицу формата [unix, mask] - Mask, [unix, y1, y2...] - All, [unix, n, y] Single. Путь к таблице в FS (.tbl, .csv). Подписи разделяются ;
    void PlotTimeline(size_t id, Text path, TMode mode, Text labels) {
        if (_beginWidget(Code::plot_time, id, labels, &path)) {
            (*p)[Code::tmode] = (uint8_t)mode;
            _endWidget();
        }
    }
    void PlotTimeline(Text path, TMode mode, Text labels) {
        PlotTimeline(_NO_ID, path, mode, labels);
    }

#ifndef SETT_NO_TABLE
    // таймлайн. Требует таблицу формата [unix, mask] - Mask, [unix, y1, y2...] - All, [unix, n, y] Single. Подписи разделяются ;
    void PlotTimeline(size_t id, ::Table& table, TMode mode, Text labels, bool clearTable = true) {
        if (_beginWidget(Code::plot, id, labels)) {
            (*p)[Code::tmode] = (uint8_t)mode;
            (*p)[Code::value];
            p->beginBin(table.writeSize());
            table.writeTo(*p);
            if (clearTable) table.removeAll();
            _endWidget();
        }
    }
    void PlotTimeline(::Table& table, TMode mode, Text labels, bool clearTable = true) {
        PlotTimeline(_NO_ID, table, mode, labels, clearTable);
    }
#endif

    // active

    // ================= JOYSTICK =================
    // флаг center - возвращать в центр при отпускании
    bool Joystick(Pos& pos, bool center = false) {
        size_t id = _next();
        if (_beginWidget(Code::joystick, id, Text())) {
            (*p)[Code::center] = center;
            _endWidget();
        }
        return _isSet(id, &pos);
    }

    // ================= INPUT =================
    // ввод текста и цифр [результат - строка], подключаемая переменная - любой тип, format - описание regex
    bool Input(size_t id, Text label = "", AnyPtr value = nullptr, Text regex = Text(), Text format = Text()) {
        if (_beginWidget(Code::input, id, label, value)) {
            if (regex) (*p)[Code::regex] = regex;
            if (format) (*p)[Code::format] = format;
            _endWidget();
        }
        return _isSet(id, value);
    }
    bool Input(Text label = "", AnyPtr value = nullptr, Text regex = Text(), Text format = Text()) {
        return Input(_next(), label, value, regex, format);
    }

    // ================= NUMBER =================
    // ввод цифр [результат - строка], подключаемая переменная - любой тип
    bool Number(size_t id, Text label = "", AnyPtr value = nullptr, float min = NAN, float max = NAN) {
        if (_beginWidget(Code::number, id, label, value)) {
            if (!isnan(min)) (*p)[Code::min] = min;
            if (!isnan(max)) (*p)[Code::max] = max;
            _endWidget();
        }
        return _isSet(id, value);
    }
    bool Number(Text label = "", AnyPtr value = nullptr, float min = NAN, float max = NAN) {
        return Number(_next(), label, value, min, max);
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
    bool Date(size_t id, Text label = "", uint32_t* value = nullptr, float zone_hours = NAN) {
        if (_beginWidget(Code::date, id, label, value)) {
            if (!isnan(zone_hours)) (*p)[Code::zone] = round(zone_hours * 60);
            _endWidget();
        }
        return _isSet(id, value);
    }
    bool Date(Text label = "", uint32_t* value = nullptr, float zone_hours = NAN) {
        return Date(_next(), label, value, zone_hours);
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
    bool DateTime(size_t id, Text label = "", uint32_t* value = nullptr, float zone_hours = NAN) {
        if (_beginWidget(Code::datetime, id, label, value)) {
            if (!isnan(zone_hours)) (*p)[Code::zone] = round(zone_hours * 60);
            _endWidget();
        }
        return _isSet(id, value);
    }
    bool DateTime(Text label = "", uint32_t* value = nullptr, float zone_hours = NAN) {
        return DateTime(_next(), label, value, zone_hours);
    }

    // ================= SPINNER =================
    // сипннер [результат - число], подключаемая переменная - любой тип
    bool Spinner(size_t id, Text label = "", float min = 0, float max = 100, float step = 1, AnyPtr value = nullptr) {
        if (_beginWidget(Code::spinner, id, label, value)) {
            (*p)[Code::min] = min;
            (*p)[Code::max] = max;
            (*p)[Code::step] = step;
            _endWidget();
        }
        return _isSet(id, value);
    }
    bool Spinner(Text label = "", float min = 0, float max = 100, float step = 1, AnyPtr value = nullptr) {
        return Spinner(_next(), label, min, max, step, value);
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
    bool Slider2(Text label = "", float min = 0, float max = 100, float step = 1, Text unit = Text(), AnyPtr value_min = nullptr, AnyPtr value_max = nullptr, uint32_t color = SETS_DEFAULT_COLOR) {
        return Slider2(_next(), _next(), label, min, max, step, unit, value_min, value_max, color);
    }
    bool Slider2(Text label, float min, float max, float step, Text unit, AnyPtr value_min, AnyPtr value_max, Colors color) {
        return Slider2(_next(), _next(), label, min, max, step, unit, value_min, value_max, (uint32_t)color);
    }

    // ================= SELECT =================
    // опции разделяются ; или \n [результат - индекс (число)]
    bool Select(size_t id, Text label, Text options, AnyPtr value = nullptr) {
        return _Select(id, label, options, value, false);
    }
    bool Select(Text label, Text options, AnyPtr value = nullptr) {
        return _Select(_next(), label, options, value, false);
    }

    // опции разделяются ; или \n [результат - строка]
    bool SelectText(Text label, Text options) {
        return _Select(_next(), label, options, nullptr, true);
    }

    bool _Select(size_t id, Text label, Text options, AnyPtr value, bool asText) {
        if (_beginWidget(Code::select, id, label, value)) {
            (*p)[Code::text] = options;
            (*p)[Code::as_txt] = asText;
            _endWidget();
        }
        return _isSet(id, value);
    }

    // ================= TABS =================
    // опции разделяются ; или \n [результат - индекс (число)]
    bool Tabs(size_t id, Text tabs, AnyPtr value = nullptr) {
        if (_beginWidget(Code::tabs, id, Text(), value)) {
            (*p)[Code::text] = tabs;
            _endWidget();
        }
        return _isSet(id, value);
    }
    bool Tabs(Text tabs, AnyPtr value = nullptr) {
        return Tabs(_next(), tabs, value);
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

    // кнопка сигналит при нажатии и отпускании, используй b.build.pressed()
    bool ButtonHold(size_t id, Text label = "", uint32_t color = SETS_DEFAULT_COLOR) {
        if (_beginWidget(Code::button, id, label, nullptr, color)) {
            (*p)[Code::button_hold] = true;
            _endWidget();
        }
        return _isSet(id, nullptr);
    }
    bool ButtonHold(Text label = "", uint32_t color = SETS_DEFAULT_COLOR) {
        return ButtonHold(_next(), label, color);
    }

    bool ButtonHold(size_t id, Text label, Colors color) {
        return ButtonHold(id, label, (uint32_t)color);
    }
    bool ButtonHold(Text label, Colors color) {
        return ButtonHold(_next(), label, (uint32_t)color);
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
                if (value.type() == AnyPtr::Type::Char && value.len()) {
                    (*p)[Code::maxlen] = value.len() - 1;
                }
            } else if (_db && p->inDB(_db, id)) {
                (*p)[Code::value];
                p->addFromDB(_db, id);
            }

            (*p)('}');
            p->checkLen();
            return _isSet(id, value);
        }
        return false;
    }

    ///////////////////////
    // ================= TABLE OLD =================
    // таблица. CSV текстом или path к файлу (разделитель ';'), заголовки разделяются ;
    void TableCSV(size_t id, Text csv, Text labels = Text()) {
        _widget(Code::table, id, labels, &csv);
    }
    void TableCSV(Text csv, Text labels = Text()) {
        TableCSV(_NO_ID, csv, labels);
    }

   private:
    void* _settings;
    sets::Packet* p = nullptr;
    void* _db = nullptr;
    size_t _auto_id = UINT32_MAX;
    int8_t _reload = 0;
    uint8_t _menuID = 0;
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

    bool _beginWidget(Code type, size_t id, const Text& label, AnyPtr value = nullptr, uint32_t color = SETS_DEFAULT_COLOR) {
        if (_enabled && build.isBuild()) {
            (*p)('{');
            (*p)[Code::type] = type;
            if (label.length()) (*p)[Code::label] = label;
            if (color != SETS_DEFAULT_COLOR) (*p)[Code::color] = color;
            if (id != _NO_ID) (*p)[Code::id] = id;

            if (value) {
                (*p)[Code::value];
                value.write(p);
                if (value.type() == AnyPtr::Type::Char && value.len()) {
                    (*p)[Code::maxlen] = value.len() - 1;
                }
            } else if (_db && id != _NO_ID && p->inDB(_db, id)) {
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

    bool _beginContainer(Code type, Text title = Text(), DivType divtype = DivType::Default) {
        if (type == Code::menu) ++_menuID;
        if (build.isBuild()) {
            (*p)('{');
            (*p)[Code::type] = type;
            if (type == Code::menu) (*p)[Code::id] = _menuID;
            if (title.length()) (*p)[Code::title] = title;
            switch (divtype) {
                case DivType::Default: break;
                case DivType::Line: (*p)[Code::divtype] = Code::line; break;
                case DivType::Block: (*p)[Code::divtype] = Code::block; break;
            }
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