#include "Script/Objects/ArrayMethods.h"
#include "Script/Objects/ArrayObject.h"
#include "Script/Objects/IntegerObject.h"
#include "Script/Objects/StringObject.h"
#include "Script/Objects/NullObject.h"
#include "Script/Objects/BuiltinFunctionObject.h"

namespace tsumugi::script::object {

std::optional<std::shared_ptr<object::IObject>> GetArrayProperty(object::ArrayObject* arrayObject, const tstring& name) {

    auto& elems = arrayObject->GetElements();

    // -------------------------
    // プロパティ
    // -------------------------
    if (name == TT("length")) {
        return std::make_shared<object::IntegerObject>(static_cast<int>(elems.size()));
    }

    // -------------------------
    // メソッド
    // -------------------------

    // push(value)
    if (name == TT("push")) {
        return std::make_shared<object::BuiltinFunctionObject>(
            [](auto receiver, const auto& args) -> std::shared_ptr<object::IObject> {
                auto arr = std::static_pointer_cast<object::ArrayObject>(receiver);
                auto& elems = arr->GetElements();

                for (auto& a : args) {
                    elems.push_back(a);
                }

                return std::make_shared<object::IntegerObject>(static_cast<int>(elems.size()));
            }
        );
    }

    // pop()
    if (name == TT("pop")) {
        return std::make_shared<object::BuiltinFunctionObject>(
            [](auto receiver, const auto&) -> std::shared_ptr<object::IObject> {
                auto arr = std::static_pointer_cast<object::ArrayObject>(receiver);
                auto& elems = arr->GetElements();

                if (elems.empty()) {
                    return object::NullObject::Instance();
                }

                auto last = elems.back();
                elems.pop_back();
                return last;
            }
        );
    }

    // slice(start, end?)
    if (name == TT("slice")) {
        return std::make_shared<object::BuiltinFunctionObject>(
            [](auto receiver, const auto& args) -> std::shared_ptr<object::IObject> {
                auto arr = std::static_pointer_cast<object::ArrayObject>(receiver);
                auto& elems = arr->GetElements();

                int start = (args.size() >= 1)
                    ? static_cast<object::IntegerObject*>(args[0].get())->GetValue()
                    : 0;

                int end = (args.size() >= 2)
                    ? static_cast<object::IntegerObject*>(args[1].get())->GetValue()
                    : static_cast<int>(elems.size());

                if (start < 0) start = 0;
                if (end < start) end = start;
                if (end > static_cast<int>(elems.size())) end = static_cast<int>(elems.size());

                std::vector<std::shared_ptr<object::IObject>> newElems(
                    elems.begin() + start,
                    elems.begin() + end
                );

                return std::make_shared<object::ArrayObject>(newElems);
            }
        );
    }

    // join(sep?)
    if (name == TT("join")) {
        return std::make_shared<object::BuiltinFunctionObject>(
            [](auto receiver, const auto& args) -> std::shared_ptr<object::IObject> {
                auto arr = std::static_pointer_cast<object::ArrayObject>(receiver);
                auto& elems = arr->GetElements();

                tstring sep = TT(",");
                if (args.size() >= 1) {
                    sep = static_cast<object::StringObject*>(args[0].get())->GetValue();
                }

                tstring result;
                for (size_t i = 0; i < elems.size(); ++i) {
                    result += elems[i]->Inspect();
                    if (i + 1 < elems.size()) result += sep;
                }

                return std::make_shared<object::StringObject>(result);
            }
        );
    }

    return std::nullopt;
}

}
