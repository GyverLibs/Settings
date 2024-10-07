#pragma once
#include <Arduino.h>

namespace sets {

enum class Code : uint8_t {
    id,
    type,
    content,
    group,
    menu,
    buttons,

    build,
    reload,
    update,
    ping,
    granted,
    fs,
    error,
    gzip,
    used,
    total,
    rssi,

    label,
    title,
    text,
    value,
    color,
    min,
    max,
    step,
    unit,

    input,
    number,
    pass,
    select,
    slider,
    toggle,
    time,
    date,
    datetime,
    button,
    paragraph,
    confirm,
    led,
};

}