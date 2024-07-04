#pragma once
#include <Arduino.h>

#define SETS_DEFAULT_COLOR 0xffffffff

namespace sets {

enum class Colors : uint32_t {
    Red = 0xcb2839,
    Orange = 0xd55f30,
    Yellow = 0xd69d27,
    Green = 0x37A93C,
    Mint = 0x25b18f,
    Aqua = 0x2ba1cd,
    Blue = 0x297bcd,
    Violet = 0x825ae7,
    Pink = 0xc8589a,
    Default = SETS_DEFAULT_COLOR,
};

}