#include "Script/Objects/ArrayObject.h"

namespace tsumugi::script::object {

ArrayObject::ArrayObject(std::vector<std::shared_ptr<IObject>> elements)
    : elements_(std::move(elements)) {

}

tstring ArrayObject::Inspect() const {

    tstring result;
    result.append(TT("["));
    for (size_t i = 0; i < elements_.size(); ++i) {
        if (i > 0) {
            result.append(TT(", "));
        }
        result.append(elements_[i]->Inspect());
    }
    result.append(TT("]"));
    return result;
}

ObjectType ArrayObject::GetType() const {

    return ObjectType::kArray;
}

}
