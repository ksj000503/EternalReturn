#pragma once
#include "CoreMinimal.h"
#include "Stat/BaseStatComponent.h"
#include "CharacterStatComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ETERNALRETURN_API UCharacterStatComponent : public UBaseStatComponent
{
    GENERATED_BODY()

public:
    UCharacterStatComponent();
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
    // ====================== 레벨 =====================
    UPROPERTY(Replicated, VisibleAnywhere, Category = "Stat")
    int32 Level = 1;

    UPROPERTY(Replicated, VisibleAnywhere, Category = "Stat")
    float CurrentEXP = 0.f;

    UPROPERTY(Replicated, VisibleAnywhere, Category = "Stat")
    float EXPToNextLevel = 0.f;

    // ================= 캐릭터 전용 스탯 =================
    UPROPERTY(Replicated, VisibleAnywhere, Category = "Stat")
    float SkillAmplification = 0.f;

    UPROPERTY(Replicated, VisibleAnywhere, Category = "Stat")
    float CooldownReduction = 0.f;

    UPROPERTY(Replicated, VisibleAnywhere, Category = "Stat")
    float BasicAttackBonus = 0.f;

    UPROPERTY(Replicated, VisibleAnywhere, Category = "Stat")
    float DamageReduction = 0.f;

    UPROPERTY(Replicated, VisibleAnywhere, Category = "Stat")
    float LifeSteal = 0.f;

    UPROPERTY(Replicated, VisibleAnywhere, Category = "Stat")
    float CriticalChance = 0.f;

    UPROPERTY(Replicated, VisibleAnywhere, Category = "Stat")
    float CriticalDamage = 150.f;

    UPROPERTY(Replicated, VisibleAnywhere, Category = "Stat")
    float VisionRange = 0.f;

    // =================== 고유 게이지 (미사용) ===================
    UPROPERTY(Replicated, VisibleAnywhere, Category = "Stat")
    float MaxGauge = 0.f;

    UPROPERTY(Replicated, VisibleAnywhere, Category = "Stat")
    float CurrentGauge = 0.f;

    // =================== 탈락 여부 ===========================
    UPROPERTY(ReplicatedUsing = OnRep_bIsEliminated, VisibleAnywhere, Category = "Stat")
    bool bIsEliminated = false;

private:
    UFUNCTION()
    void OnRep_bIsEliminated();

public:
    // ================== 레벨 ==============================
    UFUNCTION(BlueprintCallable, Category = "Stat")
    void AddEXP(float Amount);

    UFUNCTION(BlueprintCallable, Category = "Stat")
    void LevelUp();

    UFUNCTION(BlueprintCallable, Category = "Stat")
    void RecalculateStats();

    // =================== Setter ========================
    UFUNCTION(BlueprintCallable, Category = "Stat")
    void SetSkillAmplification(float Value) { SkillAmplification = Value; }

    UFUNCTION(BlueprintCallable, Category = "Stat")
    void SetCooldownReduction(float Value) { CooldownReduction = Value; }

    UFUNCTION(BlueprintCallable, Category = "Stat")
    void SetBasicAttackBonus(float Value) { BasicAttackBonus = Value; }

    UFUNCTION(BlueprintCallable, Category = "Stat")
    void SetDamageReduction(float Value) { DamageReduction = Value; }

    UFUNCTION(BlueprintCallable, Category = "Stat")
    void SetLifeSteal(float Value) { LifeSteal = Value; }

    UFUNCTION(BlueprintCallable, Category = "Stat")
    void SetCriticalChance(float Value) { CriticalChance = Value; }

    UFUNCTION(BlueprintCallable, Category = "Stat")
    void SetCriticalDamage(float Value) { CriticalDamage = Value; }

    UFUNCTION(BlueprintCallable, Category = "Stat")
    void SetVisionRange(float Value) { VisionRange = Value; }

    // =================== Getter ========================
    UFUNCTION(BlueprintPure, Category = "Stat")
    float GetSkillAmplification() const { return SkillAmplification; }

    UFUNCTION(BlueprintPure, Category = "Stat")
    float GetCooldownReduction() const { return CooldownReduction; }

    UFUNCTION(BlueprintPure, Category = "Stat")
    float GetBasicAttackBonus() const { return BasicAttackBonus; }

    UFUNCTION(BlueprintPure, Category = "Stat")
    float GetDamageReduction() const { return DamageReduction; }

    UFUNCTION(BlueprintPure, Category = "Stat")
    float GetLifeSteal() const { return LifeSteal; }

    UFUNCTION(BlueprintPure, Category = "Stat")
    float GetCriticalChance() const { return CriticalChance; }

    UFUNCTION(BlueprintPure, Category = "Stat")
    float GetCriticalDamage() const { return CriticalDamage; }

    UFUNCTION(BlueprintPure, Category = "Stat")
    float GetVisionRange() const { return VisionRange; }

    UFUNCTION(BlueprintPure, Category = "Stat")
    int32 GetLevel() const { return Level; }
};