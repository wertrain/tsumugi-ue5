#pragma once

#include "TsumugiEngine/Foundation/Types.h"
#include "TsumugiEngine/Script/Objects/IObject.h"
#include <unordered_map>
#include <optional>

namespace tsumugi::script::object { class ClassObject; }

namespace tsumugi::script::object {

// UserObject は「動的プロパティを持つオブジェクト」を表す。
// 
// 用途は2つ：
// 1. ユーザー定義クラスのインスタンス
//      class Foo {} → Foo() で生成されるインスタンス
// 2. オブジェクトリテラル
//      let o = { x: 0 } のようなリテラルも UserObject になる
//
// prototype_ によるプロトタイプチェーンを持ち、
// JavaScript の Object と同様の振る舞いをする。
// 
// ※注意：Vector2, Color などの「組み込み構造体クラス」には使用しない。
//   それらは BuiltinInstanceObject を使う。
//   UserObject は動的プロパティ辞書を持つため、
//   ネイティブ値を持つ組み込み型には適さない。

class UserObject : public IObject {
public:
    explicit UserObject();

    virtual std::shared_ptr<IObject> Get(const tstring& name) const;
    virtual void Set(const tstring& name, std::shared_ptr<IObject> value);
    std::shared_ptr<UserObject> GetPrototype() const;
    void SetPrototype(std::shared_ptr<UserObject> proto);
    const std::unordered_map<tstring, std::shared_ptr<IObject>>& GetProperties() const { return properties_; }

    tstring Inspect() const override;
    ObjectType GetType() const override;

    std::optional<std::shared_ptr<object::IObject>> TryGetProperty(const tstring& name);
    std::shared_ptr<ClassObject> GetOwnerClass() const { return ownerClass_.lock(); }
    void SetOwnerClass(const std::shared_ptr<ClassObject>& klass) { ownerClass_ = klass; }

private:
    std::unordered_map<tstring, std::shared_ptr<IObject>> properties_;
    std::shared_ptr<UserObject> prototype_;
    std::weak_ptr<ClassObject> ownerClass_;
};

}
