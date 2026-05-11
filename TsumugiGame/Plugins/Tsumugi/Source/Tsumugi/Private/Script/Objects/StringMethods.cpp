#include "Script/Objects/StringMethods.h"
#include "Script/Objects/StringObject.h"
#include "Script/Objects/IntegerObject.h"
#include "Script/Objects/BooleanObject.h"
#include "Script/Objects/BuiltinMethodObject.h"
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <cwctype>

namespace tsumugi::script::object {

std::shared_ptr<object::IObject> GetStringProperty(std::shared_ptr<object::IObject> object, const tstring& name, const common::ErrorReporter& errors) {

    auto str = static_cast<object::StringObject*>(object.get());
    const auto& s = str->GetValue();

    static const std::unordered_map<tstring,
        std::function<std::shared_ptr<object::IObject>(
            std::shared_ptr<object::IObject>,
            const std::vector<std::shared_ptr<object::IObject>>&
        )>
    > methods = {

        // -------------------------
        // substr(start, length?)
        // -------------------------
        {
            TT("substr"),
            [](std::shared_ptr<object::IObject> thisObj, const std::vector<std::shared_ptr<object::IObject>>& args)
            {
                auto s = static_cast<object::StringObject*>(thisObj.get())->GetValue();

                if (args.size() == 0) {
                    return std::make_shared<object::StringObject>(s);
                }

                int start = static_cast<object::IntegerObject*>(args[0].get())->GetValue();
                int length = (args.size() >= 2)
                    ? static_cast<object::IntegerObject*>(args[1].get())->GetValue()
                    : (int)s.size() - start;

                if (start < 0) start = 0;
                if (start > (int)s.size()) start = s.size();
                if (length < 0) length = 0;

                return std::make_shared<object::StringObject>(s.substr(start, length));
            }
        },

        // -------------------------
        // upper()
        // -------------------------
        {
            TT("upper"),
            [](std::shared_ptr<object::IObject> thisObj, const std::vector<std::shared_ptr<object::IObject>>&)
            {
                auto s = static_cast<object::StringObject*>(thisObj.get())->GetValue();
                tstring out = s;
                std::transform(out.begin(), out.end(), out.begin(), ::toupper);
                return std::make_shared<object::StringObject>(out);
            }
        },

        // -------------------------
        // lower()
        // -------------------------
        {
            TT("lower"),
            [](std::shared_ptr<object::IObject> thisObj, const std::vector<std::shared_ptr<object::IObject>>&)
            {
                auto s = static_cast<object::StringObject*>(thisObj.get())->GetValue();
                tstring out = s;
                std::transform(out.begin(), out.end(), out.begin(), ::towlower);
                return std::make_shared<object::StringObject>(out);
            }
        },

        // -------------------------
        // slice(start, end?)
        // -------------------------
        {
            TT("slice"),
            [](std::shared_ptr<object::IObject> thisObj, const std::vector<std::shared_ptr<object::IObject>>& args)
            {
                auto s = static_cast<object::StringObject*>(thisObj.get())->GetValue();

                int start = (args.size() >= 1)
                    ? static_cast<object::IntegerObject*>(args[0].get())->GetValue()
                    : 0;

                int end = (args.size() >= 2)
                    ? static_cast<object::IntegerObject*>(args[1].get())->GetValue()
                    : (int)s.size();

                if (start < 0) start = 0;
                if (end < start) end = start;
                if (end > (int)s.size()) end = s.size();

                return std::make_shared<object::StringObject>(s.substr(start, end - start));
            }
        },

        // -------------------------
        // startsWith(prefix)
        // -------------------------
        {
            TT("startsWith"),
            [](std::shared_ptr<object::IObject> thisObj, const std::vector<std::shared_ptr<object::IObject>>& args)
            {
                auto s = static_cast<object::StringObject*>(thisObj.get())->GetValue();
                auto prefix = static_cast<object::StringObject*>(args[0].get())->GetValue();

                bool result = s.rfind(prefix, 0) == 0;
                return object::BooleanObject::FromBool(result);
            }
        },

        // -------------------------
        // endsWith(suffix)
        // -------------------------
        {
            TT("endsWith"),
            [](std::shared_ptr<object::IObject> thisObj, const std::vector<std::shared_ptr<object::IObject>>& args)
            {
                auto s = static_cast<object::StringObject*>(thisObj.get())->GetValue();
                auto suffix = static_cast<object::StringObject*>(args[0].get())->GetValue();

                bool result =
                    s.size() >= suffix.size() &&
                    s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;

                return object::BooleanObject::FromBool(result);
            }
        },
    };

    // プロパティ
    if (name == TT("length")) {
        return std::make_shared<object::IntegerObject>(s.size());
    }

    // メソッド
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
