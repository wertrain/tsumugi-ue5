#pragma once

#include "TsumugiEngine/Foundation/Types.h"
#include "TsumugiEngine/Script/Objects/IObject.h"
#include <functional>

namespace tsumugi::script::object {

using BuiltinFunctionType = std::function<std::shared_ptr<object::IObject>(std::shared_ptr<object::IObject>, const std::vector<std::shared_ptr<object::IObject>>&)>;

// BuiltinFunctionObject は tsumugi における「組み込み関数」を表す。
// 
// 【役割】
// - C++ 側で定義された関数（keys, values, len, print など）を
//   スクリプトから呼び出せるようにするためのオブジェクト。
// - グローバル空間に登録され、Environment から参照される。
// - UserFunctionObject と異なり、定義時スコープ（Environment）を持たず、
//   ただ 1 つの C++ 関数ポインタ（function_）だけを保持する。
// 
// 【呼び出しモデル】
// - Evaluator は BuiltinFunctionObject を見つけると、
//     function_(receiver, args)
//   を直接呼び出す。
// - receiver はメソッド呼び出し時の this（obj.method() の obj）。
//   グローバル関数として呼ぶ場合は null が渡される。
// 
// 【設計上の意図】
// - 組み込み関数は「言語レベルの関数」ではなく、
//   「ホスト言語（C++）側の機能をスクリプトに公開するための橋渡し」。
// - UserFunctionObject（スクリプトで定義された関数）とは明確に分離し、
//   クロージャ環境や ownerClass などの概念を持たない。
// 
// 【メリット】
// - 組み込み関数の追加が容易（C++ のラムダを登録するだけ）。
// - UserFunctionObject と責務が分離され、関数モデルがシンプルになる。
// - receiver を受け取れるため、メソッド的な組み込み関数も実装可能。
// 
// これにより BuiltinFunctionObject は、
// 「C++ 側の機能をスクリプトに公開するための最小限の関数オブジェクト」
// として機能し、tsumugi の組み込み API の基盤となる。

class BuiltinFunctionObject : public IObject {
public:
    explicit BuiltinFunctionObject(BuiltinFunctionType fn);
    BuiltinFunctionType GetFunction() const { return function_; }
    tstring Inspect() const override;
    ObjectType GetType() const override;

private:
    BuiltinFunctionType function_;
};

}
