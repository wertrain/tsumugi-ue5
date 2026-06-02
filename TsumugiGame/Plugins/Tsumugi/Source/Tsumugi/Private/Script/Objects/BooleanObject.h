#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"

namespace tsumugi::script::object {

class BooleanObject : public IObject {
public:
    static std::shared_ptr<BooleanObject> True() {
        static auto instance = std::make_shared<BooleanObject>(true);
        return instance;
    }
    static std::shared_ptr<BooleanObject> False() {
        static auto instance = std::make_shared<BooleanObject>(false);
        return instance;
    }
    static std::shared_ptr<BooleanObject> FromBool(const bool value) {
        return value ? True() : False();
    }
public:
    explicit BooleanObject(bool value);
    bool GetValue() const;
    void SetValue(bool value);

    tstring Inspect() const override;
    ObjectType GetType() const override;

private:
    bool value_;
};

}
