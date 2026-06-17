#pragma once
#include "CoreMinimal.h"
#include "CombatEntityBase.h"
#include "InventoryComponent.h"
#include "Stat/CharacterStatComponent.h"
#include "EternalReturnCharacter.generated.h"

class UCraftingComponent;
class USkillComponent;
UCLASS(abstract)
class ETERNALRETURN_API AEternalReturnCharacter : public ACombatEntityBase
{
    GENERATED_BODY()

public:
    AEternalReturnCharacter();

    virtual void BeginPlay() override;

    UFUNCTION(BlueprintPure, Category = "State")
    bool IsResting() const { return bIsResting; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    EWeaponType AllowedWeaponType = EWeaponType::None;

    // 인벤토리
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    TObjectPtr<UInventoryComponent> InventoryComponent;

    // 크래프팅
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crafting")
    TObjectPtr<UCraftingComponent> CraftingComponent;

    // 스킬
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
    TObjectPtr<USkillComponent> SkillComponent;

    // CharacterStatComponent — StatComponent와 별개 인스턴스 아님
    // InitializeStats의 Setter가 이쪽으로 리다이렉트됨
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
    TObjectPtr<UCharacterStatComponent> CharacterStatComponent;

    // ─── Setter 오버라이드 (CharacterStatComponent로 리다이렉트) ──
    virtual void SetMaxHP(float value) override;
    virtual void SetCurrentHP(float value) override;
    virtual void SetHPRegen(float value) override;
    virtual void SetAttackPower(float value) override;
    virtual void SetDefense(float value) override;
    virtual void SetMoveSpeed(float value) override;
    virtual void SetAttackSpeed(float value) override;
    virtual void SetAttackRange(float value) override;

    // ─── CharacterStatComponent 전용 Setter ──
    UFUNCTION(BlueprintCallable, Category = "Stats") void SetSkillAmplification(float Value);
    UFUNCTION(BlueprintCallable, Category = "Stats") void SetCooldownReduction(float Value);
    UFUNCTION(BlueprintCallable, Category = "Stats") void SetBasicAttackBonus(float Value);
    UFUNCTION(BlueprintCallable, Category = "Stats") void SetDamageReduction(float Value);
    UFUNCTION(BlueprintCallable, Category = "Stats") void SetCriticalChance(float Value);
    UFUNCTION(BlueprintCallable, Category = "Stats") void SetCriticalDamage(float Value);
    UFUNCTION(BlueprintCallable, Category = "Stats") void SetLifeSteal(float Value);
    UFUNCTION(BlueprintCallable, Category = "Stats") void SetVisionRange(float Value);

    // ─── Getter ──
    UFUNCTION(BlueprintPure, Category = "Stats") float GetSkillAmplification() const;
    UFUNCTION(BlueprintPure, Category = "Stats") float GetCooldownReduction() const;
    UFUNCTION(BlueprintPure, Category = "Stats") float GetBasicAttackBonus() const;
    UFUNCTION(BlueprintPure, Category = "Stats") float GetDamageReduction() const;
    UFUNCTION(BlueprintPure, Category = "Stats") float GetCriticalChance() const;
    UFUNCTION(BlueprintPure, Category = "Stats") float GetCriticalDamage() const;
    UFUNCTION(BlueprintPure, Category = "Stats") float GetLifeSteal() const;
    UFUNCTION(BlueprintPure, Category = "Stats") float GetVisionRange() const;

protected:
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Character")
    int32 Gold;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Character")
    int32 Experience;

    UPROPERTY(ReplicatedUsing = OnRep_IsResting, BlueprintReadOnly, Category = "State")
    bool bIsResting;

    UFUNCTION()
    void OnRep_IsResting();

    virtual void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};