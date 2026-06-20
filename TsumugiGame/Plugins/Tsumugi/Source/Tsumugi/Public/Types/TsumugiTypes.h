#pragma once

#include "CoreMinimal.h"
#include "TsumugiTypes.generated.h"

namespace tsumugi::script::object { class IObject; }

UCLASS()
class TSUMUGI_API UTsumugiTypes : public UObject
{
    GENERATED_BODY()

public:
    // ここはあとで Vector3Instance / TransformInstance と FVector/FTransform を
    // 結びつけるためのヘルパーを追加していく想定。
};
