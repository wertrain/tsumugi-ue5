#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"

class IDetailLayoutBuilder;

/// <summary>
/// 詳細パネル（Details Panel）の表示を動的にカスタマイズするクラス
/// </summary>
class TSUMUGIEDITOR_API FTsumugiComponentDetails : public IDetailCustomization
{
public:
    /// <summary>
    /// 詳細パネル表示時にUE5のPropertyEditorシステムがこのインスタンスを作成するための静的メソッド
    /// </summary>
    /// <returns></returns>
    static TSharedRef<IDetailCustomization> MakeInstance();

    /// <summary>
    /// スクリプト側から渡ってきたメタデータを元にUIを構築します。
    /// </summary>
    /// <param name="DetailBuilder"></param>
    virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
};
