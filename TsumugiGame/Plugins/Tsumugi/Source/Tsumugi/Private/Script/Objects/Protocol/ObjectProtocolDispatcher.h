#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"
#include <optional>

namespace tsumugi::script::object::protocol {

class ObjectProtocolDispatcher {
public:
    static std::optional<std::shared_ptr<object::IObject>> TryGetProperty(std::shared_ptr<object::IObject> object, const tstring& name);
    static bool TrySetProperty(std::shared_ptr<object::IObject> object, const tstring& name, std::shared_ptr<object::IObject> value);
    static bool IsCallable(const std::shared_ptr<object::IObject>& object);
};

}
