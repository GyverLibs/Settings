#pragma once
#include <Arduino.h>
#include <StringUtils.h>

namespace sets {
class Build {
   public:
    enum class Type {
        None,
        Build,
        Set,
        Click
    };

    Build(Type type, bool granted, size_t id = 0, Text value = Text()) : _type(type), _granted(granted), _id(id), _value(value) {}

    // клиент авторизован
    bool isGranted() {
        return _granted;
    }

    // тип билда
    Type type() {
        return _type;
    }

    // тип - сборка виджетов
    bool isBuild() {
        return _type == Type::Build;
    }

    // тип - действие (обработка клика или значения)
    bool isAction() {
        return (_type == Type::Set || _type == Type::Click);
    }

    // id виджета (действие)
    size_t id() {
        return _id;
    }

    // значение виджета (действие)
    Text& value() {
        return _value;
    }

    // сбросить тип билда
    void reset() {
        _type = Type::None;
    }

   private:
    Type _type;
    bool _granted;
    size_t _id;
    Text _value;
};
}  // namespace sets