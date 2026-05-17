#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"
#include <optional>

namespace tsumugi::script::object::protocol {

// ObjectProtocolDispatcher は Monkey には存在しない tsumugi 独自の拡張。
// 
// 【役割】
//   - obj.x や obj["x"] のようなプロパティアクセスを一元的に扱う
//   - 型ごとのプロパティ取得/設定ロジックを Evaluator から分離する
//   - UserObject / 組み込み型 / クラスなど、あらゆるオブジェクトの
//     プロパティアクセスを統一的に扱う
//
// 【tsumugi における重要性】
//   Monkey には「プロパティ」という概念が存在しないため、
//   この Dispatcher は tsumugi のオブジェクトモデルの中核となる。
//   すべてのプロパティアクセスはここを経由することで、
//   言語全体の一貫性と拡張性を確保している。
//
// 【メソッド呼び出しとの関係】
//   - obj.method のように関数が返された場合、UserFunctionObject は
//     BoundMethodObject(receiver=obj, function=method) にラップされる。
//   - BuiltinFunctionObject は receiver をセットした上で返される。
//   これにより obj.method() が自然に動作する。
// 
// 【提供機能】
//   - TryGetProperty: プロパティ取得
//   - TrySetProperty: プロパティ設定（UserObject のみ許可）
//   - IsCallable: 関数・メソッド・組み込み関数かどうかの判定


class ObjectProtocolDispatcher {
public:

    // プロパティ取得
    // obj.x の評価時に呼ばれる。
    // TryGetProperty は「プロパティ値を取得する」ことだけを担当し、
    // メソッド化（UserFunctionObject → BoundMethodObject への変換）や
    // BuiltinFunctionObject への receiver の注入は行わない。
    // これらの処理は Evaluator::EvalPropertyAccessExpression 側で行われる。
    static std::optional<std::shared_ptr<object::IObject>> TryGetProperty(std::shared_ptr<object::IObject> object, const tstring& name);
    // プロパティ設定
    // obj.x = value のような代入に対応する。
    // UserObject のように「プロパティを持つ型」のみが対応し、
    // 組み込み型（Array, String など）は通常は設定不可。
    static std::optional<std::shared_ptr<object::IObject>> TrySetProperty(std::shared_ptr<object::IObject> object, const tstring& name, std::shared_ptr<object::IObject> value);
    // 呼び出し可能かどうか（関数・メソッド・組み込み関数など）
    // InvokeFunction が呼び出し前に利用する。
    // BoundMethodObject / UserFunctionObject / BuiltinFunctionObject が true を返す。
    static bool IsCallable(const std::shared_ptr<object::IObject>& object);
};

}
