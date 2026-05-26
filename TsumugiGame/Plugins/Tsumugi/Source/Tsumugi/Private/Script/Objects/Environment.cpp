#include "Script/Objects/Environment.h"
#include "Script/Objects/IObject.h"

// 以下は CreateGlobalEnvironment() のため
#include "Script/Builtins/Vector/Vector3Class.h"
#include "Script/Builtins/Vector/Vector3Instance.h"
#include "Script/Builtins/Vector/Vector2Class.h"
#include "Script/Builtins/Vector/Vector2Instance.h"
#include "Script/Builtins/Random/RandomClass.h"
#include "Script/Builtins/Random/RandomInstance.h"
#include "Script/Builtins/Quaternion/QuaternionClass.h"
#include "Script/Builtins/Quaternion/QuaternionInstance.h"
#include "Script/Objects/BuiltinClassObject.h"


// 【定義時環境（definition environment）】
// 関数が定義されたときの Environment。
// UserFunctionObject が保持する。
// クロージャを実現するために必要。
//
// 【実行時環境（execution environment）】
// InvokeFunction が関数を呼び出すときに作る新しい Environment。
// - 引数束縛
// - self の注入（メソッド呼び出し時）
// などを行う。

namespace tsumugi::script::object {

// メソッドのレシーバー名としてセットする文字列
const tstring Environment::kThis = TT("this");

// Environment()
// -----------------------------------------------------------------------------
// グローバルスコープや最初のローカルスコープを作るときに使う。
// outer_ を持たない「ルート環境」。
// -----------------------------------------------------------------------------
Environment::Environment()
    : store_()
    , outer_()
    , currentFunction_() {

}

// Environment(outer)
// -----------------------------------------------------------------------------
// 新しいスコープを作るときに使う。
// 例：
//   - 関数呼び出し時（InvokeFunction）
//   - ブロックスコープ（if, while, for）
//   - self を注入したメソッド呼び出し
//
// outer_ に親スコープを保持することで、レキシカルスコープを実現する。
// -----------------------------------------------------------------------------
Environment::Environment(std::shared_ptr<object::Environment> outer)
    : store_()
    , outer_(std::move(outer))
    , currentFunction_() {

}

// Get(name)
// -----------------------------------------------------------------------------
// 変数名 name を現在のスコープから検索し、見つからなければ outer_ を辿る。
// これは Monkey のレキシカルスコープと同じ挙動。
// 
// tsumugi では、self（メソッド呼び出し時の receiver）も Environment に
// 通常の変数として束縛されるため、Get("self") で取得される。
// 
// 例：
//   let x = 10;
//   fn() { x }  // x は outer_ を辿って見つかる
//
//   obj.method() の場合：
//     InvokeFunction が新しい Environment を作り、
//     そこに self を束縛する。
// -----------------------------------------------------------------------------
std::shared_ptr<object::IObject> Environment::Get(const tstring& name) {

    auto it = store_.find(name);
    if (it != store_.end()) {
        return it->second;
    }

    // 自分のスコープになければ親を探しに行く
    if (auto outer = outer_.lock()) {
        return outer->Get(name);
    }

    return nullptr;
}

// Set(name, value)
// -----------------------------------------------------------------------------
// 現在のスコープに name → value の束縛を追加する。
// Monkey と同じく、代入は常に「現在のスコープ」に対して行われる。
// 
// 例：
//   let x = 1;      // Set("x", 1)
//   fn() { x = 2 }  // x は outer_ にあるため、EvalAssignmentExpression 側で
//                   // outer_ を辿って更新する必要がある（tsumugi 側の実装次第）
//
// tsumugi では setter（obj.x = value）も存在するため、
// プロパティ代入は ObjectProtocolDispatcher に委譲される。
// Set は「変数代入」に限定される。
// -----------------------------------------------------------------------------
std::shared_ptr<object::IObject> Environment::Set(const tstring& name, std::shared_ptr<IObject> value) {

    // すでにこのスコープにあるなら更新
    if (store_.find(name) != store_.end()) {
        store_[name] = value;
        return value;
    }

    // 親スコープにあるなら親を更新
    if (auto outer = outer_.lock()) {
        if (outer->store_.find(name) != outer->store_.end()) {
            outer->Set(name, value);
            return value;
        }
    }

    store_[name] = value;
    return value;
}

// Clear()
// -----------------------------------------------------------------------------
// Environment が保持する変数束縛（store_）を破棄し、
// FunctionObject と Environment の循環参照を解消するためのメソッド。
// 
// 【なぜ必要か】
// UserFunctionObject は Environment を保持し、
// Environment は IObject（UserFunctionObject を含む）を保持するため、
// 循環参照が発生する可能性がある。
// 
// 通常の実行では問題ないが、REPL やスクリプト実行後に Environment を破棄する際、
// 循環参照が残るとメモリリークとして検出される。
// 
// Clear() を呼ぶことで store_ を空にし、
// Environment → IObject → Environment の循環を断ち切る。
// 
// 【呼ぶべきタイミング】
// - REPL 終了時
// - スクリプト実行後に Environment を破棄する直前
// -----------------------------------------------------------------------------
void Environment::Clear() {

    store_.clear();
}

void Environment::CreateGlobalEnvironment() {

    RegisterBuiltin<builtins::vector::Vector3Instance>(builtins::vector::CreateVector3Class);
    RegisterBuiltin<builtins::vector::Vector2Instance>(builtins::vector::CreateVector2Class);
    RegisterBuiltin<builtins::random::RandomInstance>(builtins::random::CreateRandomClass);
    RegisterBuiltin<builtins::quaternion::QuaternionInstance>(builtins::quaternion::CreateQuaternionClass);
}

}
