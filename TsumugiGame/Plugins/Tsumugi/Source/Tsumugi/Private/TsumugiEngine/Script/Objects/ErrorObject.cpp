#include "TsumugiEngine/Script/Objects/ErrorObject.h"

namespace tsumugi::script::object {

ErrorObject::ErrorObject(tstring message)
    : message_(std::move(message))
    , code_(std::nullopt)
    , placeholders_() {

}

ErrorObject::ErrorObject(ErrorCode code, std::unordered_map<std::string, tstring> placeholders)
    : message_(std::nullopt)
    , code_(code)
    , placeholders_(placeholders) {

}

bool ErrorObject::HasMessage() const {

    return message_.has_value();
}

bool ErrorObject::HasCode() const {

    return code_.has_value();
}

const tstring& ErrorObject::GetMessage() const {

    return message_.value();
}

ErrorCode ErrorObject::GetCode() const {

    return code_.value();
}

tstring ErrorObject::Inspect() const {

    return message_.value();
}

ObjectType ErrorObject::GetType() const {

    return ObjectType::kError;
}

}
