#include "Script/Objects/ArrayMethods.h"
#include "Script/Objects/ArrayObject.h"
#include "Script/Objects/IntegerObject.h"
#include "Script/Objects/StringObject.h"
#include "Script/Objects/NullObject.h"
#include "Script/Objects/BuiltinMethodObject.h"

namespace tsumugi::script::object {

std::shared_ptr<object::IObject> GetArrayProperty(std::shared_ptr<object::IObject> object, const tstring& name, const common::ErrorReporter& errors) {

    auto arr = static_cast<object::ArrayObject*>(object.get());
    auto& elements = arr->GetElements();

    // -------------------------
    // プロパティ
    // -------------------------
    if (name == TT("length")) {
        return std::make_shared<object::IntegerObject>(elements.size());
    }

    // -------------------------
    // メソッドテーブル
    // -------------------------
    using MethodType = std::function<
        std::shared_ptr<object::IObject>(
            std::shared_ptr<object::IObject>,
            const std::vector<std::shared_ptr<object::IObject>>&
        )
    >;

    static const std::unordered_map<tstring, MethodType> methods = {

        // -------------------------
        // push(value)
        // -------------------------
        {
            TT("push"),
            [](std::shared_ptr<object::IObject> thisObj, const std::vector<std::shared_ptr<object::IObject>>& args)
            {
                auto arr = static_cast<object::ArrayObject*>(thisObj.get());
                auto& elems = arr->GetElements();

                for (auto& a : args) {
                    elems.push_back(a);
                }

                return std::make_shared<object::IntegerObject>(elems.size());
            }
        },

        // -------------------------
        // pop()
        // -------------------------
        {
            TT("pop"),
            [](std::shared_ptr<object::IObject> thisObj, const std::vector<std::shared_ptr<object::IObject>>&)
            {
                auto arr = static_cast<object::ArrayObject*>(thisObj.get());
                auto& elems = arr->GetElements();

                if (elems.empty()) {
                    return std::static_pointer_cast<object::IObject>(object::NullObject::Instance());
                }

                auto last = elems.back();
                elems.pop_back();
                return last;
            }
        },

        // -------------------------
        // slice(start, end?)
        // -------------------------
        {
            TT("slice"),
            [](std::shared_ptr<object::IObject> thisObj, const std::vector<std::shared_ptr<object::IObject>>& args)
            {
                auto arr = static_cast<object::ArrayObject*>(thisObj.get());
                auto& elems = arr->GetElements();

                int start = (args.size() >= 1)
                    ? static_cast<object::IntegerObject*>(args[0].get())->GetValue()
                    : 0;

                int end = (args.size() >= 2)
                    ? static_cast<object::IntegerObject*>(args[1].get())->GetValue()
                    : elems.size();

                if (start < 0) start = 0;
                if (end < start) end = start;
                if (end > (int)elems.size()) end = elems.size();

                std::vector<std::shared_ptr<object::IObject>> newElems(
                    elems.begin() + start,
                    elems.begin() + end
                );

                return std::make_shared<object::ArrayObject>(newElems);
            }
        },

        // -------------------------
        // join(sep?)
        // -------------------------
        {
            TT("join"),
            [](std::shared_ptr<object::IObject> thisObj, const std::vector<std::shared_ptr<object::IObject>>& args)
            {
                auto arr = static_cast<object::ArrayObject*>(thisObj.get());
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
        },
    };

    // -------------------------
    // メソッド検索
    // -------------------------
    auto it = methods.find(name);
    if (it != methods.end()) {
        return std::make_shared<object::BuiltinMethodObject>(
            it->second,
            object
        );
    }

    return errors.MakeErrorObject(i18n::MessageId::kInvalidProperty, name);
}

}
