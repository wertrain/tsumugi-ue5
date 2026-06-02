#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"
#include "Script/Builtins/BuiltinTypes.h"
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

    /// <summary>
    /// 型判定に使用するため、必須とします
    /// 派生クラスでは DEFINE_BUILTIN_CLASS マクロを使用して定義してください
    /// </summary>
    /// <returns></returns>
    virtual builtin::BuiltinType GetBuiltinType() const { return builtin::BuiltinType::Num; }

    /// <summary>
    /// 自身のプロパティからメンバーを取得します
    /// </summary>
    /// <param name="name"></param>
    /// <returns></returns>
    std::shared_ptr<IObject> Get(const tstring& name) const;
    /// <summary>
    /// 自身のプロパティにメンバーを登録します
    /// </summary>
    /// <param name="name"></param>
    /// <param name="value"></param>
    void Set(const tstring& name, std::shared_ptr<IObject> value);
    /// <summary>
    /// プロトタイプを取得します
    /// </summary>
    /// <returns></returns>
    std::shared_ptr<BuiltinInstanceObject> GetPrototype() const;
    /// <summary>
    /// プロトタイプを設定します
    /// </summary>
    /// <param name="proto"></param>
    void SetPrototype(std::shared_ptr<BuiltinInstanceObject> proto);

    tstring Inspect() const override;
    ObjectType GetType() const override;

    /// <summary>
    /// プロパティを取得します
    /// 自身のプロパティとプロトタイプから検索します
    /// </summary>
    /// <param name="name"></param>
    /// <returns></returns>
    std::optional<std::shared_ptr<object::IObject>> TryGetProperty(const tstring& name);
    /// <summary>
    /// 自身のプロパティにプロパティに値を設定します
    /// このメソッドを使用した場合、型は問わずに設定できるため
    /// 基本的には派生先で意図した型かチェック、もしくは変換してセットすることを推奨します
    /// </summary>
    /// <param name="name"></param>
    /// <param name="value"></param>
    /// <returns></returns>
    virtual bool TrySetProperty(const tstring& name, std::shared_ptr<object::IObject> value);

private:
    std::unordered_map<tstring, std::shared_ptr<IObject>> properties_;
    std::shared_ptr<BuiltinInstanceObject> prototype_;
};

#define DEFINE_BUILTIN_CLASS(builtinType) \
    static inline constexpr builtin::BuiltinType StaticType = builtinType; \
    builtin::BuiltinType GetBuiltinType() const override { return StaticType; } \

template <builtin::BuiltinType T>
bool IsInstanceOf(const std::shared_ptr<IObject>& obj) {
    if (!obj) return false;
    if (obj->GetType() != ObjectType::kBuiltinInstance) return false;

    auto inst = std::static_pointer_cast<BuiltinInstanceObject>(obj);
    return inst->GetBuiltinType() == T;
}

}
