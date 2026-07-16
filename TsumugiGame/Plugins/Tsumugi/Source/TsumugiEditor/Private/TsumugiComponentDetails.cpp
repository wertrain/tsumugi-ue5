
#include "TsumugiComponentDetails.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "DetailCategoryBuilder.h"
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

        CustomCategory.AddCustomRow(FText::FromString(Var.Name))
            .NameContent()
            [
                SNew(STextBlock).Text(FText::FromString(Var.Name)).ToolTipText(FText::FromString(Var.Tooltip))
            ]
            .ValueContent()
            [
                SNew(SEditableTextBox)
                    .Text(FText::FromString(Var.Value))
                    .OnTextCommitted_Lambda([VariablesInterface, Var](const FText& NewText, ETextCommit::Type CommitType) {
                    // インターフェースを通じて値を安全に書き戻す
                    //VariablesInterface->UpdateVariableValue(Var.Name, NewText.ToString());
                 })
            ];
    }
}

/*
void FTsumugiComponentDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
    TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;
    DetailBuilder.GetObjectsBeingCustomized(ObjectsBeingCustomized);

    UTsumugiScriptComponent* VariablesInterface = nullptr;
    for (const TWeakObjectPtr<UObject>& Object : ObjectsBeingCustomized)
    {
        if (Object.IsValid() && Object->IsA<UTsumugiScriptComponent>())
        {
            VariablesInterface = Cast<UTsumugiScriptComponent>(Object.Get());
            break; // 最初の1つを対象にする場合
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

        CustomCategory.AddCustomRow(FText::FromString(Var.Name))
            .NameContent()
            [
                SNew(STextBlock).Text(FText::FromString(Var.Name)).ToolTipText(FText::FromString(Var.Tooltip))
            ]
            .ValueContent()
            [
                SNew(SEditableTextBox)
                    .Text(FText::FromString(Var.Value))
                    .OnTextCommitted_Lambda([VariablesInterface, Var](const FText& NewText, ETextCommit::Type CommitType) {
                    // インターフェースを通じて値を安全に書き戻す
                    //VariablesInterface->UpdateVariableValue(Var.Name, NewText.ToString());
                })
            ];
    }
}*/
