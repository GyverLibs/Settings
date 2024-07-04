#pragma once
#include <Arduino.h>

#include "builder.h"
#include "codes.h"

namespace sets {

class BasicContainer {
   public:
    BasicContainer(Code type, Builder& b, Text title = Text()) : b(b) {
        b._beginContainer(type, title);
    }
    ~BasicContainer() {
        b._endContainer();
    }

   private:
    Builder& b;
};

class Group : public BasicContainer {
   public:
    Group(Builder& b, Text title = Text()) : BasicContainer(Code::group, b, title) {}
};

class Menu : public BasicContainer {
   public:
    Menu(Builder& b, Text title) : BasicContainer(Code::menu, b, title) {}
};

class Buttons : public BasicContainer {
   public:
    Buttons(Builder& b) : BasicContainer(Code::buttons, b) {}
};

}  // namespace sets