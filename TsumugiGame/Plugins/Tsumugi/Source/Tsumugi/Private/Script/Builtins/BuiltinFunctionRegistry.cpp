#include "Script/Builtins/BuiltinFunctionRegistry.h"
#include "Script/Objects/ArrayObject.h"
#include "Script/Objects/HashObject.h"
#include "Script/Objects/StringObject.h"
#include "Script/Objects/IntegerObject.h"
#include "Script/Objects/BooleanObject.h"
#include "Script/Objects/NullObject.h"
#include "Script/Objects/ObjectHash.h"
#include <unordered_map>

namespace tsumugi::script::builtin {

void BuiltinFunctionRegistry::Register(const tstring& name, std::shared_ptr<object::BuiltinFunctionObject> fn) {

    functions_[name] = fn;
}

std::shared_ptr<object::BuiltinFunctionObject> BuiltinFunctionRegistry::Get(const tstring& name) {

    auto it = functions_.find(name);
    if (it != functions_.end()) {
        return it->second;
    }
    return nullptr;
}

void BuiltinFunctionRegistry::Initialize() {

    std::call_once(initialized_, []() {
        RegisterBuiltinFunctions();
    });
}

void BuiltinFunctionRegistry::RegisterBuiltinFunctions() {

    Register(TT("keys"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject> receiver, const std::vector<std::shared_ptr<object::IObject>>& args) -> std::shared_ptr<object::IObject>
            {
                if (args.size() != 1) {
                    return errors_.MakeErrorObject(i18n::MessageId::kWrongNumberOfArguments, TT("1"), std::to_string(args.size()));
                }
                if (args[0]->GetType() != object::ObjectType::kHash) {
                    return errors_.MakeErrorObject(i18n::MessageId::kTypeMismatch, args[0]->Inspect(), TT("Hash"));
                }
                auto hashObj = static_cast<object::HashObject*>(args[0].get());

                std::vector<std::shared_ptr<object::IObject>> elements;
                for (auto& kv : hashObj->GetPairs()) {
                    elements.push_back(kv.second.key);
                }

                return std::make_shared<object::ArrayObject>(elements);
            }
        )
    );

    Register(TT("values"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject> receiver, const std::vector<std::shared_ptr<object::IObject>>& args) -> std::shared_ptr<object::IObject>
            {
                if (args.size() != 1) {
                    return errors_.MakeErrorObject(i18n::MessageId::kWrongNumberOfArguments, TT("1"), std::to_string(args.size()));
                }
                if (args[0]->GetType() != object::ObjectType::kHash) {
                    return errors_.MakeErrorObject(i18n::MessageId::kTypeMismatch, args[0]->Inspect(), TT("Hash"));
                }
                auto hashObj = static_cast<object::HashObject*>(args[0].get());

                std::vector<std::shared_ptr<object::IObject>> elements;
                for (auto& kv : hashObj->GetPairs()) {
                    elements.push_back(kv.second.value);
                }
                return std::make_shared<object::ArrayObject>(elements);
            }
        )
    );

    Register(TT("len"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject> receiver, const std::vector<std::shared_ptr<object::IObject>>& args) -> std::shared_ptr<object::IObject>
            {
                if (args.size() != 1) {
                    return errors_.MakeErrorObject(i18n::MessageId::kWrongNumberOfArguments, tstring(TT("1")), std::to_string(args.size()));
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
                        return errors_.MakeErrorObject(i18n::MessageId::kTypeMismatch, obj->Inspect(), TT("String/Array/Hash"));
                }
            }
        )
    );

    Register(TT("has"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject> receiver, const std::vector<std::shared_ptr<object::IObject>>& args) -> std::shared_ptr<object::IObject>
            {
                if (args.size() != 2) {
                    return errors_.MakeErrorObject(i18n::MessageId::kWrongNumberOfArguments, tstring(TT("2")), std::to_string(args.size()));
                }
                if (args[0]->GetType() != object::ObjectType::kHash) {
                    return errors_.MakeErrorObject(i18n::MessageId::kTypeMismatch, args[0]->Inspect(), TT("Hash"));
                }

                auto hashObj = static_cast<object::HashObject*>(args[0].get());
                auto keyObj = args[1];

                object::HashKey key = MakeHashKey(keyObj.get());
                const auto& pairs = hashObj->GetPairs();
                bool exists = pairs.find(key) != pairs.end();
                return exists ? object::BooleanObject::True() : object::BooleanObject::False();
            }
        )
    );

    Register(TT("range"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject> receiver, const std::vector<std::shared_ptr<object::IObject>>& args) -> std::shared_ptr<object::IObject>
            {
                if (args.size() != 1) {
                    return errors_.MakeErrorObject(i18n::MessageId::kWrongNumberOfArguments, tstring(TT("1")), std::to_string(args.size()));
                }
                if (args[0]->GetType() != object::ObjectType::kInteger) {
                    return errors_.MakeErrorObject(i18n::MessageId::kTypeMismatch, args[0]->Inspect(), TT("Integer"));
                }

                auto intObj = static_cast<object::IntegerObject*>(args[0].get());
                int n = intObj->GetValue();
                if (n < 0) {
                    return errors_.MakeErrorObject(i18n::MessageId::kInvalidArgument, args[0]->Inspect());
                }

                std::vector<std::shared_ptr<object::IObject>> elements;
                elements.reserve(n);
                for (int i = 0; i < n; ++i) {
                    elements.push_back(std::make_shared<object::IntegerObject>(i));
                }
                return std::make_shared<object::ArrayObject>(elements);
            }
        )
    );

    Register(
        TT("print"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject> receiver, const std::vector<std::shared_ptr<object::IObject>>& args)
            -> std::shared_ptr<object::IObject>
            {
                tstring out;

                for (size_t i = 0; i < args.size(); i++) {
                    out += args[i] ? args[i]->Inspect() : TT("null");
                    if (i + 1 < args.size()) out += TT(" ");
                }
                log::tlogstring str = type::convert::TStringToString(out);

                logger_.Log(log::TextLogger::Categories::Information, str);

                return object::NullObject::Instance();
            }
        )
    );
}

void InitializeBuiltinFunctions() {

    BuiltinFunctionRegistry::Initialize();
}

}
