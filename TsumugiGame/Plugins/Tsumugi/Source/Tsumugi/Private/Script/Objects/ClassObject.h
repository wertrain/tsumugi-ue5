#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"
#include <unordered_map>
#include <optional>

namespace tsumugi::script::object { class UserObject; }

namespace tsumugi::script::object {

// ClassObject は「ユーザー定義クラス」を表すオブジェクト。
// 
// - `class Foo {}` を定義すると ClassObject が生成される。
// - `Foo()` を呼ぶと UserObject（インスタンス）が生成される。
// - prototype ベースのメソッド解決や継承（parent_）をサポートする。
// 
// ※注意：Random, Math, Vector2 などの「組み込みクラス」には使用しない。
//   組み込みクラスは BuiltinClassObject / BuiltinInstanceObject を使う。
//   UserObject はユーザー定義クラス専用のインスタンス表現であり、
//   組み込み構造体（Vector2 など）には適さない。

// prototype_ はユーザー定義クラスのインスタンス（UserObject）が
// メソッドを探索する際のプロトタイプチェーンの起点となる。
// JavaScript の Foo.prototype に相当する。

class ClassObject : public IObject {
public:
    static constexpr const tchar* kConstructorName = TT("init");
    static constexpr const tchar* kSuperKeyword = TT("super");

public:
    ClassObject(const tstring& name, std::unordered_map<tstring, std::shared_ptr<IObject>> methods, std::shared_ptr<UserObject> prototype, std::shared_ptr<ClassObject> parent);
    ClassObject(const tstring& name);

    const tstring& GetName() const { return name_; }
    std::shared_ptr<UserObject> GetPrototype() const { return prototype_; }
    std::shared_ptr<ClassObject> GetParentClass() const { return parent_; }

    void SetMethods(std::unordered_map<tstring, std::shared_ptr<IObject>> methods) { methods_ = std::move(methods); }
    void SetPrototype(std::shared_ptr<UserObject> prototype) { prototype_ = std::move(prototype); }
    void SetParent(std::shared_ptr<ClassObject> parent) { parent_ = std::move(parent); }
    void SetStaticMethod(const tstring& name, std::shared_ptr<IObject> fn) { staticMethods_[name] = fn; }

    std::optional<std::shared_ptr<IObject>> TryGetMethod(const tstring& name) const;
    std::optional<std::shared_ptr<IObject>> TryGetStaticMethod(const tstring& name) const;
    std::optional<std::shared_ptr<IObject>> TryGetProperty(const tstring& name) const;
    tstring Inspect() const override;
    ObjectType GetType() const override;

private:
    tstring name_;
    std::unordered_map<tstring, std::shared_ptr<IObject>> methods_;
    std::unordered_map<tstring, std::shared_ptr<IObject>> staticMethods_;
    std::shared_ptr<UserObject> prototype_;
    std::shared_ptr<ClassObject> parent_;
};

}
