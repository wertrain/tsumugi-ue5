#pragma once
//
// FunctionObject の設計について
//
// FunctionObject は「関数リテラルの定義時に存在した Environment」を
// environment_ として保持する。これはクロージャを正しく動作させるために
// 必須の設計である。
//
// 例：
//   let f = function(x) {
//       function(y) { x + y }
//   };
//   let g = f(2);
//   g(3); // x=2 を保持している必要がある
//
// 【environment_ は shared_ptr である理由】
// --------------------------------------------
// - クロージャは「定義時の環境」を延命する必要がある。
// - weak_ptr にすると、外側のスコープが破棄された瞬間に
//   environment_ が消えてしまい、クロージャが壊れる。
// - そのため environment_ は shared_ptr で保持する必要がある。
//
// 【循環参照が発生する理由】
// --------------------------------------------
// FunctionObject は environment_ を shared_ptr で保持する。
// 一方、Environment.store_ は変数束縛として FunctionObject を shared_ptr で保持する。
// これにより：
//
//   Environment(store_) → FunctionObject（shared）
//   FunctionObject.environment_ → Environment（shared）
//
// の 2 ノード循環が発生する。
// C++ の shared_ptr は循環参照を解決できないため、
// この状態はメモリリークとして検出される。
//
// 【循環参照の解消方法】
// --------------------------------------------
// Environment::Clear() を呼び、store_ を明示的に破棄することで
// Environment → FunctionObject の参照を切る。
// これにより循環が解消され、FunctionObject.environment_ も解放される。
//
// Clear() を呼ばない場合、循環参照が残り、
// _CrtDumpMemoryLeaks() によりリークとして報告される。
// これは C++ における shared_ptr ベースのクロージャ実装の仕様である。
//
// 【env_stack_ による代替案について】
// --------------------------------------------
// Evaluator 側でスコープをスタック管理する方式（env_stack_）を用いれば、
// Clear() を呼ばずに循環参照を自然に解消する設計も可能である。
// ただし Evaluator がステートフルになり、実装が複雑化するため、
// 本プロジェクトでは採用していない。
// 
// 本 FunctionObject は「クロージャ保持のために Environment を所有する」
// という役割に特化しており、循環参照の解消は Environment 側の責務とする。
//

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
    std::shared_ptr<object::Environment> environment_;
};

}
