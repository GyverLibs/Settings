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

    Build(Type type, bool granted, size_t id = 0, Text value = Text()) : type(type), granted(granted), id(id), value(value) {}

    // тип билда
    const Type type;

    // клиент авторизован
    const bool granted;

    // id виджета (действие)
    const size_t id;

    // значение виджета (действие)
    const Text value;

    // тип - сборка виджетов
    bool isBuild() {
        return type == Type::Build;
    }

    // тип - действие (обработка клика или значения)
    bool isAction() {
        return (type == Type::Set || type == Type::Click);
    }
};

}  // namespace sets