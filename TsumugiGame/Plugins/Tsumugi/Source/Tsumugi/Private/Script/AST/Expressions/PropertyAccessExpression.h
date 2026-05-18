#pragma once

#include "Foundation/Types.h"
#include "Script/AST/IExpression.h"

namespace tsumugi::script::lexer { class Token; }
namespace tsumugi::script::ast::expression { class Identifier; }

namespace tsumugi::script::ast::expression {

// PropertyAccessExpression
// -----------------------------------------------------------------------------
// Monkey には存在しない、tsumugi 独自の構文要素。
//
//   obj.x
//
// のような「ドットによるプロパティアクセス」を表す。
//
// 【構文上の位置づけ】
// - obj["x"] を表す IndexExpression とは別の AST ノード。
// - つまり糖衣構文ではなく、構文レベルで明確に区別される。
//
// 【意味論上の扱い】
// - 実際のプロパティ取得は ObjectProtocolDispatcher に委譲される。
// - Dispatcher は「値を返すだけ」で、メソッド化は行わない。
// - Evaluator::EvalPropertyAccessExpression 側で、
//     - UserFunctionObject → BoundMethodObject にラップ
//     - BuiltinFunctionObject → receiver をセット
//   することで obj.method() が自然に動作する。
//
// 【拡張性】
// - UserObject（将来のクラス/インスタンス）とも自然に統合される。
// - obj.method のようなメソッド呼び出しモデルの基盤となる。
//
// tsumugi のプロパティアクセスモデルの中心となる AST ノード。
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
