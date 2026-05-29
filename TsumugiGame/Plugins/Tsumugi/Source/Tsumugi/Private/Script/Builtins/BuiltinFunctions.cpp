#include "Script/Builtins/BuiltinFunctions.h"
#include "Script/Objects/ArrayObject.h"
#include "Script/Objects/HashObject.h"
#include "Script/Objects/StringObject.h"
#include "Script/Objects/IntegerObject.h"
#include "Script/Objects/BooleanObject.h"
#include "Script/Objects/ObjectHash.h"
#include "Common/ErrorReporter.h"
#include <unordered_map>

namespace tsumugi::script::builtin {

static std::unordered_map<tstring, std::shared_ptr<object::BuiltinFunctionObject>> g_builtinFunctions;
static common::ErrorReporter g_errors;

std::once_flag g_builtinInitialized;

void InitializeBuiltinFunctionsOld() {

    std::call_once(g_builtinInitialized, []() {

    g_builtinFunctions[TT("keys")] =
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject> receiver, const std::vector<std::shared_ptr<object::IObject>>& args) -> std::shared_ptr<object::IObject>
            {
                if (args.size() != 1) {
                    return g_errors.MakeErrorObject(i18n::MessageId::kWrongNumberOfArguments, TT("1"), std::to_string(args.size()));
                }
                if (args[0]->GetType() != object::ObjectType::kHash) {
                    return g_errors.MakeErrorObject(i18n::MessageId::kTypeMismatch, args[0]->Inspect(), TT("Hash"));
                }
                auto hashObj = static_cast<object::HashObject*>(args[0].get());

                std::vector<std::shared_ptr<object::IObject>> elements;
                for (auto& kv : hashObj->GetPairs()) {
                    elements.push_back(kv.second.key);
                }

                return std::make_shared<object::ArrayObject>(elements);
            }
        );

    g_builtinFunctions[TT("values")] =
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject> receiver, const std::vector<std::shared_ptr<object::IObject>>& args) -> std::shared_ptr<object::IObject>
            {
                if (args.size() != 1) {
                    return g_errors.MakeErrorObject(i18n::MessageId::kWrongNumberOfArguments, TT("1"), std::to_string(args.size()));
                }
                if (args[0]->GetType() != object::ObjectType::kHash) {
                    return g_errors.MakeErrorObject(i18n::MessageId::kTypeMismatch, args[0]->Inspect(), TT("Hash"));
                }
                auto hashObj = static_cast<object::HashObject*>(args[0].get());

                std::vector<std::shared_ptr<object::IObject>> elements;
                for (auto& kv : hashObj->GetPairs()) {
                    elements.push_back(kv.second.value);
                }
                return std::make_shared<object::ArrayObject>(elements);
            }
        );

    g_builtinFunctions[TT("len")] =
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject> receiver, const std::vector<std::shared_ptr<object::IObject>>& args) -> std::shared_ptr<object::IObject>
            {
                if (args.size() != 1) {
                    return g_errors.MakeErrorObject(i18n::MessageId::kWrongNumberOfArguments, tstring(TT("1")), std::to_string(args.size()));
                }

                auto& obj = args[0];

                switch (obj->GetType()) {

                    case object::ObjectType::kString: {
                        auto strObj = static_cast<object::StringObject*>(obj.get());
                        return std::make_shared<object::IntegerObject>(
                            static_cast<int>(strObj->GetValue().size())
                        );
                    }

                    case object::ObjectType::kArray: {
                        auto arrObj = static_cast<object::ArrayObject*>(obj.get());
                        return std::make_shared<object::IntegerObject>(
                            static_cast<int>(arrObj->GetElements().size())
                        );
                    }

                    case object::ObjectType::kHash: {
                        auto hashObj = static_cast<object::HashObject*>(obj.get());
                        return std::make_shared<object::IntegerObject>(
                            static_cast<int>(hashObj->GetPairs().size())
                        );
                    }

                    default:
                        return g_errors.MakeErrorObject(i18n::MessageId::kTypeMismatch, obj->Inspect(), TT("String/Array/Hash"));
                }
            }
        );

    g_builtinFunctions[TT("has")] =
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject> receiver, const std::vector<std::shared_ptr<object::IObject>>& args) -> std::shared_ptr<object::IObject>
            {
                if (args.size() != 2) {
                    return g_errors.MakeErrorObject(i18n::MessageId::kWrongNumberOfArguments, tstring(TT("2")), std::to_string(args.size()));
                }
                if (args[0]->GetType() != object::ObjectType::kHash) {
                    return g_errors.MakeErrorObject(i18n::MessageId::kTypeMismatch, args[0]->Inspect(), TT("Hash"));
                }

                auto hashObj = static_cast<object::HashObject*>(args[0].get());
                auto keyObj = args[1];

                object::HashKey key = MakeHashKey(keyObj.get());
                const auto& pairs = hashObj->GetPairs();
                bool exists = pairs.find(key) != pairs.end();
                return exists ? object::BooleanObject::True() : object::BooleanObject::False();
            }
        );

    g_builtinFunctions[TT("range")] =
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject> receiver, const std::vector<std::shared_ptr<object::IObject>>& args) -> std::shared_ptr<object::IObject>
            {
                if (args.size() != 1) {
                    return g_errors.MakeErrorObject(i18n::MessageId::kWrongNumberOfArguments, tstring(TT("1")), std::to_string(args.size()));
                }
                if (args[0]->GetType() != object::ObjectType::kInteger) {
                    return g_errors.MakeErrorObject(i18n::MessageId::kTypeMismatch, args[0]->Inspect(), TT("Integer"));
                }

                auto intObj = static_cast<object::IntegerObject*>(args[0].get());
                int n = intObj->GetValue();
                if (n < 0) {
                    return g_errors.MakeErrorObject(i18n::MessageId::kInvalidArgument, args[0]->Inspect());
                }

                std::vector<std::shared_ptr<object::IObject>> elements;
                elements.reserve(n);
                for (int i = 0; i < n; ++i) {
                    elements.push_back(std::make_shared<object::IntegerObject>(i));
                }
                return std::make_shared<object::ArrayObject>(elements);
            }
        );

    });
}

std::shared_ptr<object::BuiltinFunctionObject> GetBuiltinFunctionByName(const tstring& name) {

    auto it = g_builtinFunctions.find(name);
    if (it != g_builtinFunctions.end()) {
        return it->second;
    }
    return nullptr;
}

}
