#pragma once
#include <Arduino.h>

#include "builder.h"
#include "codes.h"

namespace sets {

// контейнер гостевого доступа
class GuestAccess {
   public:
    GuestAccess(Builder& b) : b(b) {
        b.beginGuest();
    }
    ~GuestAccess() {
        b.endGuest();
    }

   private:
    Builder& b;
};

class BasicContainer {
   public:
    BasicContainer(Code type, Builder& b, Text title = Text(), DivType divtype = DivType::Default) : b(b) {
        b._beginContainer(type, title, divtype);
    }
    ~BasicContainer() {
        b._endContainer();
    }

   private:
    Builder& b;
};

// контейнер группы виджетов
class Group : public BasicContainer {
   public:
    Group(Builder& b, Text title = Text()) : BasicContainer(Code::group, b, title) {}
};

// контейнер вложенного меню
class Menu : public BasicContainer {
   public:
    Menu(Builder& b, Text title) : BasicContainer(Code::menu, b, title) {}
};

// контейнер кнопок
class Buttons : public BasicContainer {
   public:
    Buttons(Builder& b) : BasicContainer(Code::buttons, b) {}
};

// горизонтальный контейнер [DivType::Line | DivType::Block]
class Row : public BasicContainer {
   public:
    Row(Builder& b, Text title = Text(), DivType divtype = DivType::Default) : BasicContainer(Code::row, b, title, divtype) {}
};

}  // namespace sets