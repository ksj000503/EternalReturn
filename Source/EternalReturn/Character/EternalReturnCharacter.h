#pragma once

#include "CoreMinimal.h"
#include "CombatEntityBase.h"
#include "EternalReturnCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

/**
 * 플레이어 캐릭터 베이스 클래스
 * 카메라, 캐릭터 전용 스탯, 재화, 상태 관리
 */
UCLASS(abstract)
class ETERNALRETURN_API AEternalReturnCharacter : public ACombatEntityBase
{
    GENERATED_BODY()

private:
    // ─── 카메라 ─────────────────────────────────────

    /** 탑다운 카메라 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UCameraComponent> TopDownCameraComponent;

    /** 카메라 거리/각도 조절용 스프링암 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USpringArmComponent> CameraBoom;

protected:
    // ─── 캐릭터 전용 스탯 ───────────────────────────

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
    float SkillAmplification;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
    float CooldownReduction;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
    float BasicAttackBonus;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
    float DamageReduction;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
    float CriticalChance;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
    float CriticalDamage;

    // ─── 재화 ───────────────────────────────────────

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Character")
    int32 Gold;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Character")
    int32 Experience;

    // ─── 상태 ───────────────────────────────────────

    UPROPERTY(ReplicatedUsing = OnRep_IsResting, BlueprintReadOnly, Category = "State")
    bool bIsResting;

    UFUNCTION()
    void OnRep_IsResting();

public:
    AEternalReturnCharacter();

    // ─── Getter ─────────────────────────────────────

    UFUNCTION(BlueprintPure, Category = "Stats")
    bool IsResting() const { return bIsResting; }

    UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent.Get(); }
    USpringArmComponent* GetCameraBoom()             const { return CameraBoom.Get(); }

protected:
    virtual void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};