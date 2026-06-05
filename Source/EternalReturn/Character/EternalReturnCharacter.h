#pragma once

#include "CoreMinimal.h"
#include "CombatEntityBase.h"
#include "InventoryComponent.h"
#include "Stat/CharacterStatComponent.h"
#include "EternalReturnCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS(abstract)
class ETERNALRETURN_API AEternalReturnCharacter : public ACombatEntityBase
{
    GENERATED_BODY()

public:
    AEternalReturnCharacter();

    UFUNCTION(BlueprintPure, Category = "State")
    bool IsResting() const { return bIsResting; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    EWeaponType AllowedWeaponType = EWeaponType::None;

    UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent.Get(); }
    USpringArmComponent* GetCameraBoom() const { return CameraBoom.Get(); }

    // ─── Setter (BP의 InitializeStats에서 그대로 호출) ──
    UFUNCTION(BlueprintCallable, Category = "Stats") void SetSkillAmplification(float Value);
    UFUNCTION(BlueprintCallable, Category = "Stats") void SetCooldownReduction(float Value);
    UFUNCTION(BlueprintCallable, Category = "Stats") void SetBasicAttackBonus(float Value);
    UFUNCTION(BlueprintCallable, Category = "Stats") void SetDamageReduction(float Value);
    UFUNCTION(BlueprintCallable, Category = "Stats") void SetCriticalChance(float Value);
    UFUNCTION(BlueprintCallable, Category = "Stats") void SetCriticalDamage(float Value);
    UFUNCTION(BlueprintCallable, Category = "Stats") void SetLifeSteal(float Value);
    UFUNCTION(BlueprintCallable, Category = "Stats") void SetVisionRange(float Value);

    // ─── Getter ─────────────────────────────────────
    UFUNCTION(BlueprintPure, Category = "Stats") float GetSkillAmplification() const;
    UFUNCTION(BlueprintPure, Category = "Stats") float GetCooldownReduction() const;
    UFUNCTION(BlueprintPure, Category = "Stats") float GetBasicAttackBonus() const;
    UFUNCTION(BlueprintPure, Category = "Stats") float GetDamageReduction() const;
    UFUNCTION(BlueprintPure, Category = "Stats") float GetCriticalChance() const;
    UFUNCTION(BlueprintPure, Category = "Stats") float GetCriticalDamage() const;
    UFUNCTION(BlueprintPure, Category = "Stats") float GetLifeSteal() const;
    UFUNCTION(BlueprintPure, Category = "Stats") float GetVisionRange() const;

protected:
    // ─── 카메라 ─────────────────────────────────────
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UCameraComponent> TopDownCameraComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    TObjectPtr<UInventoryComponent> InventoryComponent;

    // ─── CharacterStatComponent ──────────────────────
    // CombatEntityBase의 StatComponent(BaseStatComponent)와 별개
    // 캐릭터 전용 스탯 관리
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
    TObjectPtr<UCharacterStatComponent> CharacterStatComponent;

    // ─── 재화 (추후 PlayerState로 이동 예정) ─────────
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Character")
    int32 Gold;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Character")
    int32 Experience;

    // ─── 상태 ───────────────────────────────────────
    UPROPERTY(ReplicatedUsing = OnRep_IsResting, BlueprintReadOnly, Category = "State")
    bool bIsResting;

    UFUNCTION()
    void OnRep_IsResting();

    virtual void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};