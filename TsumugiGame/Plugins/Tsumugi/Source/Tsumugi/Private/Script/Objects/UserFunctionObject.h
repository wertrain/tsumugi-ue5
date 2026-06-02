#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"
#include <functional>

namespace tsumugi::script::ast::expression { class Identifier; }
namespace tsumugi::script::ast::statement { class BlockStatement; }
namespace tsumugi::script::object { class Environment; }
namespace tsumugi::script::object { class ClassObject; }

namespace tsumugi::script::object {

// UserFunctionObject は Monkey の FunctionObject に相当し、
// tsumugi における「純粋な関数」を表す。
// 
// 【設計上の意図】
// - UserFunctionObject は「定義時の環境（Environment）」だけを保持し、
//   this/self の概念を内部には持たない。
// - メソッド呼び出し時の this の注入は Evaluator 側で行われ、
//   UserFunctionObject 自体は “関数としての本体” のみを担当する。
// 
// 【メソッドとして使われる場合】
// - ClassObject に登録された UserFunctionObject は「メソッド本体」として扱われる。
// - 呼び出し時には Evaluator が receiver（this）を新しい Environment に束縛し、
//   その環境で UserFunctionObject の body を実行する。
// - つまり、UserFunctionObject は「メソッド化の仕組み」を持たず、
//   Evaluator が関数を “メソッドとして実行する” 役割を担う。
// 
// 【super 対応のための拡張】
// - メソッドがどのクラスに属しているかを知る必要があるため、
//   UserFunctionObject は ownerClass_ を保持する。
// - super.foo() の解決は Evaluator 側で行われ、
//   「現在実行中のメソッドの ownerClass → 親クラス → prototype」
//   という階層を辿ってメソッドを検索する。
// - UserFunctionObject 自体は super の解決ロジックを持たず、
//   あくまで “定義時スコープを持つ純粋な関数” として振る舞う。
// 
// 【メリット】
// - 関数オブジェクトを汚さず、関数とメソッドの責務を明確に分離できる。
// - this / super / 継承といった OOP の仕組みを Evaluator と ClassObject に閉じ込められる。
// - Monkey の「関数はただの値」という哲学を保ちつつ、
//   クラスベース OOP を自然に拡張できる。
// 
// これにより UserFunctionObject は、
// 「定義時スコープを持つ純粋な関数」
// かつ
// 「クラス階層に属するメソッドの本体」
// の両方を安全に担う、tsumugi の関数・メソッドモデルの基盤となる。

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
    std::shared_ptr<object::ClassObject> GetOwnerClass() const { return ownerClass_; }
    void SetOwnerClass(std::shared_ptr<object::ClassObject> c) { ownerClass_ = c; }

    tstring Inspect() const override;
    ObjectType GetType() const override;

private:
    std::vector<std::shared_ptr<ast::expression::Identifier>> parameters_;
    std::shared_ptr<const ast::statement::BlockStatement> body_;
    std::shared_ptr<object::Environment> environment_;
    std::shared_ptr<object::ClassObject> ownerClass_;
};

}
