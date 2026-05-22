#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"
#include <optional>

namespace tsumugi::script::object::protocol {

// ObjectProtocolDispatcher
// -----------------------------------------------------------------------------
// Monkey には存在しない、tsumugi 独自のオブジェクトモデルを支える
// 「プロパティアクセスの統一インターフェース」。
// 
// obj.x や obj["x"] のようなプロパティアクセスを、型に依存せず
// 一元的に扱うためのディスパッチャである。
//
// 【役割】
// - UserObject / 組み込み型 / クラス / 配列 / 文字列など、
//   あらゆるオブジェクトのプロパティ取得・設定を統一的に扱う。
// - Evaluator から型分岐を排除し、オブジェクトごとのプロトコルを
//   ここに集約することで、言語全体の一貫性と拡張性を確保する。
//
// 【tsumugi の意味論における位置づけ】
// - TryGetProperty は「プロパティ値を返すだけ」を担当し、
//   メソッド化（this の注入）は行わない。
//   → obj.foo() / super.foo() のレシーバ決定は
//      EvalCallExpression → Invoke に集約される。
// - TrySetProperty は UserObject など「プロパティを持つ型」だけが対応し、
//   組み込み型は通常は設定不可。
// - IsCallable は「呼び出し可能かどうか」を判定し、Invoke が利用する。
//
// 【提供機能】
// - TryGetProperty: プロパティ取得
// - TrySetProperty: プロパティ設定
// - IsCallable: 呼び出し可能オブジェクトかどうかの判定
//
// tsumugi のオブジェクトモデルにおける「プロパティアクセスの中核」となる
// コンポーネントであり、Evaluator と各オブジェクト型の橋渡しを行う。
// -----------------------------------------------------------------------------

class ObjectProtocolDispatcher {
public:

    // プロパティ取得
    // obj.x の評価時に呼ばれる。
    // TryGetProperty は「プロパティ値を取得する」ことだけを担当する
    static std::optional<std::shared_ptr<object::IObject>> TryGetProperty(std::shared_ptr<object::IObject> object, const tstring& name);
    // プロパティ設定
    // obj.x = value のような代入に対応する。
    // UserObject のように「プロパティを持つ型」のみが対応し、組み込み型（Array, String など）は通常は設定不可。
    static std::optional<std::shared_ptr<object::IObject>> TrySetProperty(std::shared_ptr<object::IObject> object, const tstring& name, std::shared_ptr<object::IObject> value);
    // 呼び出し可能かどうか（関数・メソッド・組み込み関数など）
    // Invoke が呼び出し前に利用する。
    static bool IsCallable(const std::shared_ptr<object::IObject>& object);
};

}
