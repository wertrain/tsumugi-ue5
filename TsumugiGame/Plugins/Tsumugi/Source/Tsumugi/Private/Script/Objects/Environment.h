#pragma once
#include "Foundation/Types.h"
#include <unordered_map>

namespace tsumugi::script::object { class IObject; }
namespace tsumugi::script::object { class UserFunctionObject; }

namespace tsumugi::script::object {

// Environment
// -----------------------------------------------------------------------------
// tsumugi の Environment は、Monkey と同様に
// 「変数名 → 値（IObject）」の束縛を保持する辞書であり、
// レキシカルスコープとクロージャを実現する中核コンポーネントである。
//
// 【Monkey と共通する点】
// - store_ にローカル変数を保持する
// - outer_ に親スコープを保持し、レキシカルスコープを形成する
// - Get(name) は現在のスコープから親スコープへと順に探索する
//
// 【tsumugi 独自の拡張点】
// - メソッド呼び出し時の「this（receiver）」の束縛も Environment が担当する
//   （this は Invoke によって実行時スコープへ注入される）
// - super 呼び出しのために「現在実行中の関数（currentFunction）」を保持する
//   → currentFunction.ownerClass / parentClass を辿ることで super を解決する
// - PropertyAccess は値を返すだけとし、this の注入は CallExpression → Invoke に集約
//
// 【循環参照について】
// - UserFunctionObject は Environment を保持し、Environment は IObject を保持するため
//   循環参照が発生し得る。
// - Clear() によって store_ を破棄することで循環参照を解消できる。
//   （REPL 終了時などに必ず呼ぶ必要がある）
//
// tsumugi の実行モデルにおいて Environment は、
// 「関数の定義時スコープ」と「関数の実行時スコープ」をつなぐ最重要コンポーネントである。
// -----------------------------------------------------------------------------
class Environment {
public:
    /// <summary>
    /// メソッドのレシーバー名としてセットする文字列
    /// </summary>
    static const tstring kThis;

public:
    explicit Environment();
    Environment(std::shared_ptr<Environment> outer);

    std::shared_ptr<object::IObject> Get(const tstring& name);
    std::shared_ptr<object::IObject> Set(const tstring& name, std::shared_ptr<object::IObject> value);

    /// <summary>
    /// Environment が保持する変数束縛（store_）を破棄し、
    /// FunctionObject と Environment の循環参照を解消する。
    /// 
    /// REPL 終了時など Environment を破棄する前に必ず呼ぶこと。
    /// 呼ばない場合、循環参照が残りメモリリークとして報告される。
    /// </summary>
    void Clear();

    // super 解決のための現在実行中の関数（循環参照防止のため生ポインタ）
    void SetCurrentFunction(UserFunctionObject* fn) { currentFunction_ = fn; }
    UserFunctionObject* GetCurrentFunction() const { return currentFunction_; }

private:
    std::unordered_map<tstring, std::shared_ptr<object::IObject>> store_;
    std::weak_ptr<Environment> outer_; // 親スコープ

    object::UserFunctionObject* currentFunction_; // ★ weak_ptr ではなく生ポインタ
};

}
