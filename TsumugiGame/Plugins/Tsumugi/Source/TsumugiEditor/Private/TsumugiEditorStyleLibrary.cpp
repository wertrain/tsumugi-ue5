#include "TsumugiEditorStyleLibrary.h"
#include "Styling/AppStyle.h"

FSlateBrush UTsumugiEditorStyleLibrary::GetBrush(FName BrushName)
{
    if (const FSlateBrush* Brush = FAppStyle::Get().GetBrush(BrushName))
    {
        return *Brush;
    }

    return FSlateBrush();
}

TArray<FName> UTsumugiEditorStyleLibrary::GetBrushNames()
{
    TArray<FName> Result;

    const ISlateStyle& Style = FAppStyle::Get();

    for (const FName& Key : Style.GetStyleKeys())
    {
        const FSlateBrush* Brush = Style.GetBrush(Key);

        // 無効なBrushを除外
        if (!Brush)
        {
            continue;
        }

        if (Brush->DrawAs == ESlateBrushDrawType::NoDrawType)
        {
            continue;
        }

        Result.Add(Key);
    }

    Result.Sort([](const FName& A, const FName& B)
    {
        return A.LexicalLess(B);
    });

    return Result;
}

TArray<UTsumugiEditorBrushInfo*> UTsumugiEditorStyleLibrary::GetBrushInfos()
{
    TArray<UTsumugiEditorBrushInfo*> Result;

    const ISlateStyle& Style = FAppStyle::Get();

    for (const FName& Key : Style.GetStyleKeys())
    {
        const FSlateBrush* Brush = Style.GetBrush(Key);

        if (!Brush)
        {
            continue;
        }

        // 描画できないBrushは除外
        if (Brush->DrawAs == ESlateBrushDrawType::NoDrawType)
        {
            continue;
        }

        UTsumugiEditorBrushInfo* Info = NewObject<UTsumugiEditorBrushInfo>();

        Info->Name = Key;
        Info->Brush = *Brush;

        Result.Add(Info);
    }

    Result.Sort([](const UTsumugiEditorBrushInfo& A, const UTsumugiEditorBrushInfo& B)
    {
        return A.Name.LexicalLess(B.Name);
    });

    return Result;
}

