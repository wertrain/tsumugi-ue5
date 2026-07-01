#pragma once

#include "TsumugiEngine/Foundation/Types.h"
#include "TsumugiEngine/Script/Objects/IObject.h"
#include <optional>
#include <unordered_map>

namespace tsumugi::script::object {

enum class ErrorCode : int32_t {
    kTypeMismatch,
    kInvalidArgument,
    kInvalidProperty,
};

/// <summary>
/// エラーを評価したときのオブジェクト
/// メッセージを直接持つパターンと、エラーコードとして保持するパターンがある
/// </summary>
class ErrorObject : public IObject {
public:
    explicit ErrorObject(tstring message);
    ErrorObject(ErrorCode code, std::unordered_map<std::string, tstring> placeholders);
    bool HasMessage() const;
    bool HasCode() const;
    const tstring& GetMessage() const;
    ErrorCode GetCode() const;
    tstring Inspect() const override;
    ObjectType GetType() const override;

private:
    std::optional<tstring> message_;
    std::optional<ErrorCode> code_;
    std::unordered_map<std::string, tstring> placeholders_;
};

}
