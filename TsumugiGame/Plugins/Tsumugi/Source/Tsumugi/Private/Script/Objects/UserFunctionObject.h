#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"
#include <functional>

namespace tsumugi::script::ast::expression { class Identifier; }
namespace tsumugi::script::ast::statement { class BlockStatement; }
namespace tsumugi::script::object { class Environment; }

namespace tsumugi::script::object {

// UserFunctionObject は Monkey の FunctionObject に相当するが、
// tsumugi では「将来的にメソッド化（BoundMethod 化）される可能性」を考慮している。
// そのため、Function 自体は receiver を持たず、
// 「定義時の環境（Environment）」だけを保持する純粋な関数オブジェクトとして設計している。
// 
// メソッド呼び出し時には、PropertyAccess や BoundMethodObject が
// この UserFunctionObject をラップし、receiver（self）を注入する。
// → これにより Function 自体を汚さず、Monkey の “wrap, don’t reinvent” の思想を維持できる。

class UserFunctionObject : public IObject {
public:
    // parameters + body + environment の組み合わせは Monkey と同じ。
    // environment は「定義時のスコープ」を保持するためのクロージャ環境。
    explicit UserFunctionObject();
    explicit UserFunctionObject(std::vector<std::shared_ptr<ast::expression::Identifier>> parameters, std::shared_ptr<ast::statement::BlockStatement> body, std::shared_ptr<object::Environment> environment);

    const std::vector<std::shared_ptr<ast::expression::Identifier>>& GetParameters() const { return parameters_; }
    void SetParameters(const std::vector<std::shared_ptr<ast::expression::Identifier>>& parameters) { parameters_ = parameters; }
    void AddParameter(std::shared_ptr<ast::expression::Identifier> parameter) { parameters_.push_back(std::move(parameter)); }
    const std::shared_ptr<const ast::statement::BlockStatement>& GetBody() const { return body_; }
    void SetBody(std::shared_ptr<const ast::statement::BlockStatement> body) { body_ = std::move(body); }
    std::shared_ptr<object::Environment> GetEnvironment() const { return environment_; }
    void SetEnvironment(std::shared_ptr<object::Environment> environment) { environment_ = std::move(environment); }

    tstring Inspect() const override;
    ObjectType GetType() const override;

private:
    std::vector<std::shared_ptr<ast::expression::Identifier>> parameters_;
    std::shared_ptr<const ast::statement::BlockStatement> body_;
    std::shared_ptr<object::Environment> environment_;
};

}
