#pragma once
#include <inttypes.h>

namespace sets {

struct Pos {
    int16_t x, y;

    operator bool() {
        return _changed ? _changed = false, true : false;
    }

    bool _changed = false;
};

}  // namespace sets