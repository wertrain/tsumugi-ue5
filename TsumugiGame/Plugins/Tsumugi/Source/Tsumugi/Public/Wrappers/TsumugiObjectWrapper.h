#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TsumugiObjectWrapper.generated.h"

namespace tsumugi::script::object { class IObject; }

UCLASS(BlueprintType)
class TSUMUGI_API UTsumugiObjectWrapper : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY()
    UObject* Target = nullptr;

    // ここに「UObject を tsumugi の UserObject に変換する」などの処理を
    // あとで追加していく想定。
};
