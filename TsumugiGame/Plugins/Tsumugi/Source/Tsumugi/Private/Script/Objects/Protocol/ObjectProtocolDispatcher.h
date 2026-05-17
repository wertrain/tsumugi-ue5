#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"
#include <optional>

namespace tsumugi::script::object::protocol {

// ObjectProtocolDispatcher は Monkey には存在しない tsumugi 独自の拡張。
// 
// 役割：
//   - obj.x や obj["x"] のようなプロパティアクセスを一元的に扱う
//   - 型ごとのプロパティ取得/設定ロジックを Evaluator から分離する
//   - 将来の UserObject / クラス / プロトコル / メソッド呼び出しに対応する
//
// Monkey は「プロパティ」という概念がないため、
// この Dispatcher は tsumugi のオブジェクトモデルの中核となる。
// 
// また、メソッド呼び出し時には：
//   obj.method → BoundMethodObject(receiver=obj, function=method)
// のようにラップする処理もここに追加される予定。

class ObjectProtocolDispatcher {
public:
    // プロパティ取得
    // obj.x の評価時に呼ばれる。
    // 将来的には「関数を返す場合は BoundMethod にラップする」処理もここに入る。
    static std::optional<std::shared_ptr<object::IObject>> TryGetProperty(std::shared_ptr<object::IObject> object, const tstring& name);
    // プロパティ設定
    // obj.x = value のような代入に対応する。
    static std::optional<std::shared_ptr<object::IObject>> TrySetProperty(std::shared_ptr<object::IObject> object, const tstring& name, std::shared_ptr<object::IObject> value);
    // 呼び出し可能かどうか（関数・メソッド・組み込み関数など）
    // InvokeFunction が呼び出し前に利用する。
    static bool IsCallable(const std::shared_ptr<object::IObject>& object);
};

}
