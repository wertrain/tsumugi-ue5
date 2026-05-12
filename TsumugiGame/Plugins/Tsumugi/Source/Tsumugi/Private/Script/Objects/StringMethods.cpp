#include "Script/Objects/StringMethods.h"
#include "Script/Objects/StringObject.h"
#include "Script/Objects/IntegerObject.h"
#include "Script/Objects/BooleanObject.h"
#include "Script/Objects/BuiltinFunctionObject.h"
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <cwctype>

namespace tsumugi::script::object {

std::optional<std::shared_ptr<object::IObject>> GetStringProperty(object::StringObject* stringObject, const tstring& name) {

    const auto& s = stringObject->GetValue();

    // -------------------------
    // プロパティ
    // -------------------------
    if (name == TT("length")) {
        return std::make_shared<object::IntegerObject>(static_cast<int>(s.size()));
    }

    // -------------------------
    // メソッド
    // -------------------------

    if (name == TT("substr")) {
        return std::make_shared<object::BuiltinFunctionObject>(
            [](auto receiver, const auto& args) -> std::shared_ptr<object::IObject> {
                auto s = std::static_pointer_cast<object::StringObject>(receiver)->GetValue();

                if (args.size() == 0) {
                    return std::make_shared<object::StringObject>(s);
                }

                int start = static_cast<object::IntegerObject*>(args[0].get())->GetValue();
                int length = (args.size() >= 2)
                    ? static_cast<object::IntegerObject*>(args[1].get())->GetValue()
                    : (int)s.size() - start;

                if (start < 0) start = 0;
                if (start > static_cast<int>(s.size())) start = static_cast<int>(s.size());
                if (length < 0) length = 0;

                return std::make_shared<object::StringObject>(s.substr(start, length));
            }
        );
    }

    if (name == TT("upper")) {
        return std::make_shared<object::BuiltinFunctionObject>(
            [](auto receiver, const auto&) {
                auto s = std::static_pointer_cast<object::StringObject>(receiver)->GetValue();
                tstring out = s;
                std::transform(out.begin(), out.end(), out.begin(), ::toupper);
                return std::make_shared<object::StringObject>(out);
            }
        );
    }

    if (name == TT("lower")) {
        return std::make_shared<object::BuiltinFunctionObject>(
            [](auto receiver, const auto&) {
                auto s = std::static_pointer_cast<object::StringObject>(receiver)->GetValue();
                tstring out = s;
                std::transform(out.begin(), out.end(), out.begin(), ::towlower);
                return std::make_shared<object::StringObject>(out);
            }
        );
    }

    if (name == TT("slice")) {
        return std::make_shared<object::BuiltinFunctionObject>(
            [](auto receiver, const auto& args) {
                auto s = std::static_pointer_cast<object::StringObject>(receiver)->GetValue();

                int start = (args.size() >= 1)
                    ? static_cast<object::IntegerObject*>(args[0].get())->GetValue()
                    : 0;

                int end = (args.size() >= 2)
                    ? static_cast<object::IntegerObject*>(args[1].get())->GetValue()
                    : static_cast<int>(s.size());

                if (start < 0) start = 0;
                if (end < start) end = start;
                if (end > static_cast<int>(s.size())) end = static_cast<int>(s.size());

                return std::make_shared<object::StringObject>(s.substr(start, end - start));
            }
        );
    }

    if (name == TT("startsWith")) {
        return std::make_shared<object::BuiltinFunctionObject>(
            [](auto receiver, const auto& args) {
                auto s = std::static_pointer_cast<object::StringObject>(receiver)->GetValue();
                auto prefix = static_cast<object::StringObject*>(args[0].get())->GetValue();

                bool result = s.rfind(prefix, 0) == 0;
                return object::BooleanObject::FromBool(result);
            }
        );
    }

    if (name == TT("endsWith")) {
        return std::make_shared<BuiltinFunctionObject>(
            [](auto receiver, const auto& args) {
                auto s = std::static_pointer_cast<object::StringObject>(receiver)->GetValue();
                auto suffix = static_cast<object::StringObject*>(args[0].get())->GetValue();

                bool result =
                    s.size() >= suffix.size() &&
                    s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;

                return object::BooleanObject::FromBool(result);
            }
        );
    }

    return std::nullopt;
}

}
