#pragma once
#include <memory>
#include <unordered_map>
#include <vector>
#include <optional>
#include "TsumugiEngine/Foundation/Types.h"

namespace tsumugi::script::object { class IObject; }

namespace tsumugi::script::analyzer {

struct AttributeInfo {
    tstring Name;
    std::vector<std::shared_ptr<object::IObject>> Arguments;
    std::vector<std::shared_ptr<object::IObject>> PositionalArguments;
    std::unordered_map<tstring, std::shared_ptr<object::IObject>> NamedArguments;
};

struct VariableMetadata {
    tstring Name;
    std::vector<AttributeInfo> Attributes;
    std::shared_ptr<object::IObject> DefaultValue;
    std::optional<tstring> TypeAnnotation;
};

struct FunctionMetadata {
    tstring Name;
    std::vector<AttributeInfo> Attributes;
};

struct ClassMetadata {
    tstring Name;
    std::vector<AttributeInfo> Attributes;
};

class ScriptMetadata {
public:
    std::vector<VariableMetadata> Variables;
    std::vector<FunctionMetadata> Functions;
    std::vector<ClassMetadata> Classes;
};

}
