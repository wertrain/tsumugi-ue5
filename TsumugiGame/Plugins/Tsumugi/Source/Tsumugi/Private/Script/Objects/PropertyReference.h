#pragma once
#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"

namespace tsumugi::script::object {

// PropertyReference は「プロパティへの参照」を表すオブジェクト。
// 
// 例：
//   let ref = obj.x;
//   ref.value   → 実際の値
//   ref.receiver → obj
//
// これは Monkey には存在しない概念で、tsumugi のプロパティアクセスを
// 「値の取得」と「参照の保持」に分離するための仕組み。
// 
// 将来的には：
//   - プロパティの setter
//   - メソッド呼び出し時の receiver の保持
//   - obj.method のようなメソッドバインディング
// に利用される。
// 
// BoundMethodObject を導入する場合、
// PropertyReference は「関数を見つけたら BoundMethod に変換する」
// という役割も担う可能性がある。

class PropertyReference : public IObject {
public:
    explicit PropertyReference(std::shared_ptr<IObject> receiver, const tstring& name, std::shared_ptr<IObject> value);

    const std::shared_ptr<object::IObject>& GetReceiver() const { return receiver_; }
    const std::shared_ptr<object::IObject>& GetValue() const { return value_; }

    tstring Inspect() const override;
    ObjectType GetType() const override;

private:
    std::shared_ptr<object::IObject> receiver_;
    std::shared_ptr<object::IObject> value_;
    tstring name_;
};

}
