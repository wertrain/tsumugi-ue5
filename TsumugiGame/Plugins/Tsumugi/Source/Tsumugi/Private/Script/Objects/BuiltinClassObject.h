#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"
#include <unordered_map>
#include <optional>
#include <functional>

namespace tsumugi::script::object { class BuiltinInstanceObject; }

namespace tsumugi::script::object {

// BuiltinClassObject は「組み込みクラス」を表すオブジェクト。
// 
// - Random, Math のような static-only クラス
// - Vector3, Transform のような instance-based クラス
// 
// の両方を扱えるように設計されている。
// 
// 【static-only モード】
//   - prototype を持たない
//   - インスタンス生成不可（Random(), Math() はエラー）
//   - static メソッドのみを提供する
// 
// 【instance-based モード】
//   - prototype（BuiltinInstanceObject）を持つ
//   - クラス呼び出しでインスタンスを生成できる（Vector3(1,2,3)）
//   - instance メソッドと static メソッドの両方を持てる
// 
// ※ユーザー定義クラス（class Foo {}）には ClassObject を使用する。
//   BuiltinClassObject は継承チェーンには参加しない（現状の仕様）。
//   将来的に組み込みクラスの継承をサポートする場合は、
//   ClassObject と構造を統合する必要がある。

class BuiltinClassObject : public IObject {
public:
    BuiltinClassObject(const tstring_view& name);

    const tstring& GetName() const { return name_; }
    void SetStaticMethod(const tstring& name, std::shared_ptr<IObject> fn) { staticMethods_[name] = fn; }
    void SetInstanceMethod(const tstring& name, std::shared_ptr<IObject> fn);
    void SetInstanceCreator(std::function<std::shared_ptr<IObject>(const std::vector<std::shared_ptr<IObject>>&)> creator);
    bool CanInstantiate() const;
    std::shared_ptr<BuiltinInstanceObject> GetPrototype() const { return prototype_; }
    std::optional<std::shared_ptr<IObject>> Instantiate(const std::vector<std::shared_ptr<IObject>>& arguments) const;
    std::optional<std::shared_ptr<IObject>> TryGetProperty(const tstring& name) const;
    tstring Inspect() const override;
    ObjectType GetType() const override;

public:
    // 派生先のインスタンス生成に使用する（主に確実に SetPrototype させるためのもの）
    //template <typename T = object::BuiltinInstanceObject, typename... Args>
    //std::shared_ptr<T> CreateInstance(Args&&... args) const {
    //    auto instance = std::make_shared<T>(std::forward<Args>(args)...);
    //    if (prototype_) {
    //        instance->SetPrototype(prototype_);
    //    }
    //    return instance;
    //}

private:
    void EnsurePrototype();

private:
    tstring name_;
    std::unordered_map<tstring, std::shared_ptr<IObject>> staticMethods_;
    std::shared_ptr<BuiltinInstanceObject> prototype_; // null の場合は static-only モード
    std::function<std::shared_ptr<IObject>(const std::vector<std::shared_ptr<IObject>>&)> instanceCreator_;
};

}
