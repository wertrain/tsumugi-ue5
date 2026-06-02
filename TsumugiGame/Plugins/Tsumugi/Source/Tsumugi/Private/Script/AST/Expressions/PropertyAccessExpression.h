#pragma once

#include "Foundation/Types.h"
#include "Script/AST/IExpression.h"

namespace tsumugi::script::lexer { class Token; }
namespace tsumugi::script::ast::expression { class Identifier; }

namespace tsumugi::script::ast::expression {

// PropertyAccessExpression
// -----------------------------------------------------------------------------
// tsumugi における「ドットによるプロパティアクセス」を表す AST ノード。
// 
//     obj.x
// 
// のような構文を扱う。Monkey には存在しない、tsumugi 独自の構文要素。
//
// 【構文上の位置づけ】
// - obj["x"] を表す IndexExpression とは別ノードとして定義される。
// - つまり糖衣構文ではなく、構文レベルで明確に区別される。
//
// 【意味論上の扱い】
// - プロパティの取得そのものは ObjectProtocolDispatcher に委譲され、
//   Dispatcher は「値を返すだけ」を担当する。
// - メソッド化（this の注入）は PropertyAccess では行わない。
// - obj.foo() / super.foo() のような「呼び出し時のレシーバ決定」は
//   EvalCallExpression 側で行われ、Invoke が receiver を受け取って実行する。
// 
// 【拡張性】
// - UserObject（クラス/インスタンス）とも自然に統合され、
//   obj.method() / super.method() のようなメソッド呼び出しモデルの基盤となる。
// - プロパティアクセスは「値を取るだけ」に徹するため、
//   呼び出しモデルの一貫性と拡張性が高い。
// 
// tsumugi のオブジェクトモデルにおける、最も基本的な構文ノードのひとつ。
// -----------------------------------------------------------------------------
class PropertyAccessExpression : public IExpression {
public:
    PropertyAccessExpression(std::shared_ptr<lexer::Token> token, std::unique_ptr<ast::IExpression> left);
    PropertyAccessExpression(std::shared_ptr<lexer::Token> token, std::unique_ptr<ast::IExpression> left, std::unique_ptr<ast::expression::Identifier> name);
    ~PropertyAccessExpression() override;

    const lexer::Token* GetToken() const { return token_.get(); }
    std::shared_ptr<lexer::Token> GetTokenShared() const { return token_; }
    void SetToken(std::shared_ptr<lexer::Token> token) { token_ = std::move(token); }
    const ast::IExpression* GetLeft() const { return left_.get(); }
    void SetLeft(std::unique_ptr<ast::IExpression> value) { left_ = std::move(value); }
    const ast::expression::Identifier* GetName() const { return name_.get(); }
    void SetName(std::unique_ptr<ast::expression::Identifier> value) { name_ = std::move(value); }

    NodeType GetNodeType() const final override { return NodeType::kPropertyAccessExpression; }
    tstring TokenLiteral() const final override;
    tstring ToCode() const final override;

private:
    std::shared_ptr<lexer::Token> token_;
    std::unique_ptr<ast::IExpression> left_;
    std::unique_ptr<ast::expression::Identifier> name_;
};

}
