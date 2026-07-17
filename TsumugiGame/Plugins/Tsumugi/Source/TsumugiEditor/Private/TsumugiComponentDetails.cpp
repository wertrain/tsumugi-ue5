
#include "TsumugiComponentDetails.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "DetailCategoryBuilder.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Runtime/TsumugiVariablesInterface.h" 

TSharedRef<IDetailCustomization> FTsumugiComponentDetails::MakeInstance()
{
    return MakeShareable(new FTsumugiComponentDetails);
}

// TsumugiEditor/Private/TsumugiComponentDetails.cpp

void FTsumugiComponentDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
    TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;
    DetailBuilder.GetObjectsBeingCustomized(ObjectsBeingCustomized);

    ITsumugiVariablesInterface* VariablesInterface = nullptr;
    for (const TWeakObjectPtr<UObject>& Object : ObjectsBeingCustomized)
    {
        if (Object.IsValid() && Object->GetClass()->ImplementsInterface(UTsumugiVariablesInterface::StaticClass()))
        {
            VariablesInterface = Cast<ITsumugiVariablesInterface>(Object.Get());
            break;
        }
    }

    if (!VariablesInterface)
    {
        return;
    }

    // 必要に応じてここで最新情報に解析（アセット読み込み時などに一回走らせる設計でも可）
    VariablesInterface->AnalyzeScriptVariables();

    // インターフェース経由で変数情報を取得してUI化
    const TArray<FStringExposedVariable>& ExposedVariables = VariablesInterface->GetExposedVariables();

    for (const FStringExposedVariable& Var : ExposedVariables)
    {
        IDetailCategoryBuilder& CustomCategory = DetailBuilder.EditCategory(
            FName(*Var.Category),
            FText::FromString(Var.Category)
        );

        // ラムダ式用に対象の変数名をキャプチャ用に確保
        FString TargetVarName = Var.Name;

        // Slateのコンテンツ（型によって切り替える）
        TSharedRef<SWidget> ValueWidget = SNullWidget::NullWidget;

        switch (Var.Type)
        {
            case ETsumugiVariableType::Boolean:
            {
                // --- 1. Boolean 用のチェックボックス (SCheckBox) ---
                ValueWidget = SNew(SCheckBox)
                    .IsChecked_Lambda([VariablesInterface, TargetVarName]() -> ECheckBoxState
                    {
                        // コンポーネント側から現在の値を取得してチェック状態を返す
                        FString CurrentValue = VariablesInterface->GetVariableValue(TargetVarName);
                        return CurrentValue.ToBool() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
                    })
                    .OnCheckStateChanged_Lambda([VariablesInterface, TargetVarName](ECheckBoxState NewState)
                    {
                        FString NewValueStr = (NewState == ECheckBoxState::Checked) ? TEXT("true") : TEXT("false");
                        VariablesInterface->UpdateVariableValue(TargetVarName, NewValueStr);
                    });
                break;
            }

            case ETsumugiVariableType::Integer:
            {
                // --- 2. 数値用のスピンボックス (SSpinBox) ---
                ValueWidget = SNew(SSpinBox<int32>)
                    .Value_Lambda([VariablesInterface, TargetVarName]() -> int32
                    {
                        FString CurrentValue = VariablesInterface->GetVariableValue(TargetVarName);
                        return FCString::Atoi(*CurrentValue);
                    })
                    .OnValueChanged_Lambda([VariablesInterface, TargetVarName](int32 NewValue)
                    {
                        // ドラッグ中など、リアルタイムに値を更新
                        VariablesInterface->UpdateVariableValue(TargetVarName, FString::FromInt(NewValue));
                    });
                break;
            }

            case ETsumugiVariableType::String:
            default:
            {
                // --- 3. 文字列・その他のためのテキストボックス (SEditableTextBox) ---
                ValueWidget = SNew(SEditableTextBox)
                    .Text_Lambda([VariablesInterface, TargetVarName]() -> FText
                    {
                        return FText::FromString(VariablesInterface->GetVariableValue(TargetVarName));
                    })
                    .OnTextCommitted_Lambda([VariablesInterface, TargetVarName](const FText& NewText, ETextCommit::Type CommitType)
                    {
                        VariablesInterface->UpdateVariableValue(TargetVarName, NewText.ToString());
                    });
                break;
            }
        }

        // 行を追加
        CustomCategory.AddCustomRow(FText::FromString(Var.Name))
            .NameContent()
            [
                SNew(STextBlock)
                    .Text(FText::FromString(Var.Name))
                    .ToolTipText(FText::FromString(Var.Tooltip))
            ]
            .ValueContent()
            [
                ValueWidget
            ];
    }
}
