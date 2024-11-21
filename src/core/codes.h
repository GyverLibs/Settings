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
    row,

    build,
    reload,
    update,
    update_tout,
    slider_tout,
    request_tout,
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
    color_on,
    color_off,

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
    alert,
    notice,
};

}