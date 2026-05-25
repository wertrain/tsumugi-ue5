#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"
#include <unordered_map>
#include <optional>

namespace tsumugi::script::object { class BuiltinClassObject; }

namespace tsumugi::script::object {

// BuiltinInstanceObject は「組み込みクラスのインスタンス」を表す。
// 
// 例：
//   let v = Vector3(1, 2, 3)
//   v.x
//   v.length()
// 
// 特徴：
// - 動的プロパティ辞書（properties_）を持つ
// - prototype によるメソッド探索をサポートする
// - UserObject と似ているが、super やユーザー定義クラスとは無関係
// - Vector3, Transform, Color などの「組み込み構造体」のインスタンスに使用する
// 
// ※ユーザー定義クラスのインスタンスには UserObject を使用する。
//   BuiltinInstanceObject は組み込み型専用であり、
//   継承チェーンや super 解決はサポートしない。

class BuiltinInstanceObject : public IObject {
public:
    explicit BuiltinInstanceObject();

    std::shared_ptr<IObject> Get(const tstring& name) const;
    void Set(const tstring& name, std::shared_ptr<IObject> value);
    std::shared_ptr<BuiltinInstanceObject> GetPrototype() const;
    void SetPrototype(std::shared_ptr<BuiltinInstanceObject> proto);
    tstring Inspect() const override;
    ObjectType GetType() const override;

    std::optional<std::shared_ptr<object::IObject>> TryGetProperty(const tstring& name);
    virtual bool TrySetProperty(const tstring& name, std::shared_ptr<object::IObject> value);

private:
    std::unordered_map<tstring, std::shared_ptr<IObject>> properties_;
    std::shared_ptr<BuiltinInstanceObject> prototype_;
};

}
