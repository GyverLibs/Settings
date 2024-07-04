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

    Build(Type type, size_t id = 0, Text value = Text()) : _type(type), _id(id), _value(value) {}

    bool isLoad() {
        return _type == Type::Build;
    }
    bool isAction() {
        return (_type == Type::Set || _type == Type::Click);
    }
    Type type() {
        return _type;
    }
    size_t id() {
        return _id;
    }
    Text& value() {
        return _value;
    }

    void reset() {
        _type = Type::None;
    }

   private:
    Type _type;
    size_t _id;
    Text _value;
};
}  // namespace sets