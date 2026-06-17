#pragma once

#include "TsumugiEngine/Foundation/Types.h"
#include "TsumugiEngine/Script/Objects/IObject.h"

namespace tsumugi::script::object {

class ErrorObject : public IObject {
public:
    explicit ErrorObject(tstring message);
    const tstring& GetMessage() const;
    tstring Inspect() const override;
    ObjectType GetType() const override;
private:
    tstring message_;
};

}
