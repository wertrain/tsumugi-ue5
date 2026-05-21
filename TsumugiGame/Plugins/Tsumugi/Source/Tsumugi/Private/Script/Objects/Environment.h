#pragma once
#include "Foundation/Types.h"
#include <unordered_map>

// Environment
// -----------------------------------------------------------------------------
// tsumugi の Environment は Monkey の Environment と同じく、
// 「変数名 → 値（IObject）」の束縛を保持する辞書であり、
// クロージャ（関数が定義されたときのスコープ）を実現するための中核となる。
//
// 【Monkey との共通点】
// - store_ にローカル変数を保持する
// - outer_ に親スコープを保持する（レキシカルスコープ）
// - Get(name) は現在のスコープから親スコープへと順に探索する
//
// 【tsumugi 独自の拡張点】
// - メソッド呼び出し時に「self（receiver）」を注入するためのスコープとしても使われる
// - BoundMethodObject と組み合わせて「定義時環境 + 実行時環境」を構築する
// - PropertyAccess / InvokeFunction と密接に連携する
//
// 【循環参照について】
// - UserFunctionObject は Environment を保持し、Environment は IObject を保持するため、
//   循環参照が発生する可能性がある。
// - Clear() を呼ぶことで store_ を破棄し、循環参照を解消する。
//   （REPL 終了時などに必ず呼ぶ必要がある）
//
// tsumugi の実行モデルにおいて Environment は「関数の定義時スコープ」と
// 「関数の実行時スコープ」をつなぐ最重要コンポーネントである。
// -----------------------------------------------------------------------------

namespace tsumugi::script::object { class IObject; }
namespace tsumugi::script::object { class UserFunctionObject; }

namespace tsumugi::script::object {

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
