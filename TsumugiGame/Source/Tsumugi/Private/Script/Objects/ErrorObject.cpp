#include "Script/Objects/ErrorObject.h"

namespace tsumugi::script::object {

ErrorObject::ErrorObject(tstring message)
    : message_(std::move(message)) {

}

const tstring& ErrorObject::GetMessage() const {

    return message_;
}

tstring ErrorObject::Inspect() const {

    return message_;
}

ObjectType ErrorObject::GetType() const {

    return ObjectType::kError;
}

}
