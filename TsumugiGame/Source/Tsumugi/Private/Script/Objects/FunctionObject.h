#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"

namespace tsumugi::script::ast::expression { class Identifier; }
namespace tsumugi::script::ast::statement { class BlockStatement; }
namespace tsumugi::script::object { class Environment; }

namespace tsumugi::script::object {

class FunctionObject : public IObject {
public:
    explicit FunctionObject();

    const std::vector<std::shared_ptr<ast::expression::Identifier>>& GetParameters() const { return parameters_; }
    void SetParameters(const std::vector<std::shared_ptr<ast::expression::Identifier>>& parameters) { parameters_ = parameters; }
    void AddParameter(std::shared_ptr<ast::expression::Identifier> parameter) { parameters_.push_back(std::move(parameter)); }
    const std::shared_ptr<const ast::statement::BlockStatement>& GetBody() const { return body_; }
    void SetBody(std::shared_ptr<const ast::statement::BlockStatement> body) { body_ = std::move(body); }
    std::shared_ptr<object::Environment> GetEnvironment() const;
    void SetEnvironment(std::shared_ptr<object::Environment> environment);

    tstring Inspect() const override;
    ObjectType GetType() const override;

private:
    std::vector<std::shared_ptr<ast::expression::Identifier>> parameters_;
    std::shared_ptr<const ast::statement::BlockStatement> body_;
    std::weak_ptr<object::Environment> environment_;
};

}
