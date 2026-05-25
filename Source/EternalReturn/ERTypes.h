#pragma once

#include "CoreMinimal.h"
#include "ERTypes.generated.h"

UENUM(BlueprintType)
enum class EStatusEffect : uint8
{
    None      UMETA(DisplayName = "없음"),
    Stun      UMETA(DisplayName = "기절"),
    Root      UMETA(DisplayName = "속박"),
    Silence   UMETA(DisplayName = "침묵"),
    Slow      UMETA(DisplayName = "감속"),
    Freeze    UMETA(DisplayName = "빙결"),
    Blind     UMETA(DisplayName = "실명"),
    Burn      UMETA(DisplayName = "화상"),
    Bleed     UMETA(DisplayName = "출혈"),
    Poison    UMETA(DisplayName = "중독"),
};