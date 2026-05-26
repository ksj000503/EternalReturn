// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CombatEntityBase.h"
#include "EternalReturnCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

/**
 *  A controllable top-down perspective character
 */
UCLASS(abstract)
class AEternalReturnCharacter : public ACombatEntityBase
{
	GENERATED_BODY()

private:
    // ─── 카메라 ─────────────────────────────────────

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UCameraComponent> TopDownCameraComponent;

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

    // ─── 캐릭터 전용 재화 ───────────────────────────

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Character")
    int32 Gold;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Character")
    int32 Experience;

    // ─── 캐릭터 전용 상태 ───────────────────────────

    UPROPERTY(ReplicatedUsing = OnRep_IsResting, BlueprintReadOnly, Category = "State")
    bool bIsResting;

    // ─── 리플리케이션 콜백 ──────────────────────────

    UFUNCTION()
    void OnRep_IsResting();

public:
    AEternalReturnCharacter();

    virtual void BeginPlay() override;

    // ─── Getter ─────────────────────────────────────


    UFUNCTION(BlueprintPure, Category = "Stats")
    bool IsResting() const { return bIsResting; }

    // ─── 카메라 Getter ───────────────────────────────

    UCameraComponent* GetTopDownCameraComponent() const 
    { 
        return TopDownCameraComponent.Get(); 
    }
    USpringArmComponent* GetCameraBoom() const 
    { 
        return CameraBoom.Get(); 
    }


    virtual void Tick(float DeltaSeconds) override;

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
