#pragma once

#include "CoreMinimal.h"
#include "TsumugiEngine/Script/Objects/UserObject.h"
#include <optional>

namespace tsumugi::integration {

/// <summary>
/// Tsumugi スクリプトから UnrealEngine の UObject に
/// プロパティアクセス・関数呼び出しを行うためのアクセサ。
/// 
/// UObject のラップを行い、Tsumugi の UserObject として
/// 統一的なアクセスインターフェイスを提供する。
/// </summary>
class UObjectAccessor : public tsumugi::script::object::UserObject
{
public:
    explicit UObjectAccessor(UObject* InTarget);

    /// <summary>
    /// Tsumugi からのプロパティ/メソッドアクセス要求を処理する。
    /// Unreal 側のプロパティまたは UFunction を解決して返す。
    /// </summary>
    /// <param name="name"></param>
    /// <returns></returns>
    virtual std::optional<std::shared_ptr<script::object::IObject>> TryGetProperty(const tstring& name) const override;

    /// <summary>
    /// Tsumugi からのプロパティ設定要求を処理する。
    /// Unreal 側のプロパティに値を反映する。
    /// </summary>
    /// <param name="name"></param>
    /// <param name="value"></param>
    virtual void Set(const tstring& name, std::shared_ptr<script::object::IObject> value) override;

    /// <summary>
    /// ラップしている Unreal の UObject を返す。
    /// </summary>
    UObject* GetTarget() const { return Target; }

    /// <summary>
    /// 
    /// </summary>
    /// <returns></returns>
    tstring Inspect() const override;

private:
    UObject* Target;

    /// <summary>
    /// Unreal 側の UPROPERTY を取得し、Tsumugi の IObject に変換して返す。
    /// </summary>
    /// <param name="name"></param>
    /// <returns></returns>
    std::shared_ptr<script::object::IObject> GetUEProperty(const tstring& name) const;

    /// <summary>
    /// Unreal 側の UPROPERTY に Tsumugi の値を設定する。
    /// </summary>
    /// <param name="name"></param>
    /// <param name="value"></param>
    /// <returns></returns>
    bool SetUEProperty(const tstring& name, const std::shared_ptr<script::object::IObject>& value);

    /// <summary>
    /// Unreal の UFunction を取得し、Tsumugi から呼び出せる関数オブジェクトとして返す。
    /// </summary>
    /// <param name="name"></param>
    /// <returns></returns>
    std::shared_ptr<script::object::IObject> GetUFunction(const tstring& name) const;

    /// <summary>
    /// Tsumugi の引数オブジェクトを Unreal の FProperty に書き込むために変換する。
    /// </summary>
    /// <param name="Property"></param>
    /// <param name="PropertyData"></param>
    /// <param name="Arg"></param>
    /// <returns></returns>
    bool ConvertArgument(class FProperty* Property, uint8* PropertyData, std::shared_ptr<script::object::IObject> Arg) const;

    /// <summary>
     /// Unreal の FProperty の値を Tsumugi の IObject に変換して返す。
    /// </summary>
    /// <param name="Property"></param>
    /// <param name="PropertyData"></param>
    /// <returns></returns>
    std::shared_ptr<script::object::IObject> ConvertPropertyValue(class FProperty* Property, uint8* PropertyData) const;
};

} // namespace tsumugi::integration
