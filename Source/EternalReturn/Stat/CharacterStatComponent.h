#pragma once
#include "CoreMinimal.h"
#include "Stat/BaseStatComponent.h"
#include "ERTypes.h" // ← 추가: FS_ItemStatData 사용을 위해 (경로 맞게 수정)
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

    // ================= 아이템 보너스 스탯 (추가) =================
    // 장착된 아이템 스탯 합산값 ? 장착 시 더하고, 해제 시 빼는 방식
    // 실제 전투 계산은 GetFinal~() 사용
    UPROPERTY(Replicated, VisibleAnywhere, Category = "Stat|Item")
    float ItemMaxHP = 0.f;

    UPROPERTY(Replicated, VisibleAnywhere, Category = "Stat|Item")
    float ItemHPRegen = 0.f;

    UPROPERTY(Replicated, VisibleAnywhere, Category = "Stat|Item")
    float ItemAttackPower = 0.f;

    UPROPERTY(Replicated, VisibleAnywhere, Category = "Stat|Item")
    float ItemDefense = 0.f;

    UPROPERTY(Replicated, VisibleAnywhere, Category = "Stat|Item")
    float ItemMoveSpeed = 0.f;

    UPROPERTY(Replicated, VisibleAnywhere, Category = "Stat|Item")
    float ItemAttackSpeed = 0.f;

    UPROPERTY(Replicated, VisibleAnywhere, Category = "Stat|Item")
    float ItemAttackRange = 0.f;

    UPROPERTY(Replicated, VisibleAnywhere, Category = "Stat|Item")
    float ItemSkillAmplification = 0.f;

    UPROPERTY(Replicated, VisibleAnywhere, Category = "Stat|Item")
    float ItemCooldownReduction = 0.f;

    UPROPERTY(Replicated, VisibleAnywhere, Category = "Stat|Item")
    float ItemBasicAttackBonus = 0.f;

    UPROPERTY(Replicated, VisibleAnywhere, Category = "Stat|Item")
    float ItemDamageReduction = 0.f;

    UPROPERTY(Replicated, VisibleAnywhere, Category = "Stat|Item")
    float ItemLifeSteal = 0.f;

    UPROPERTY(Replicated, VisibleAnywhere, Category = "Stat|Item")
    float ItemCriticalChance = 0.f;

    UPROPERTY(Replicated, VisibleAnywhere, Category = "Stat|Item")
    float ItemCriticalDamage = 0.f;

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

    // ================= 아이템 스탯 적용/제거 (추가) =================
    // EquipItem 후 → ApplyItemStats 호출
    // UnequipItem 후 → RemoveItemStats 호출
    UFUNCTION(BlueprintCallable, Category = "Stat")
    void ApplyItemStats(const FS_ItemStatData& Stats);

    UFUNCTION(BlueprintCallable, Category = "Stat")
    void RemoveItemStats(const FS_ItemStatData& Stats);

    // ================= Final Getter (추가) =================
    // 기본 스탯 + 아이템 보너스 합산 ? 전투/이동 계산에 반드시 이걸 사용
    UFUNCTION(BlueprintPure, Category = "Stat")
    float GetFinalMaxHP() const { return MaxHP + ItemMaxHP; }

    UFUNCTION(BlueprintPure, Category = "Stat")
    float GetFinalHPRegen() const { return HPRegen + ItemHPRegen; }

    UFUNCTION(BlueprintPure, Category = "Stat")
    float GetFinalAttackPower() const { return AttackPower + ItemAttackPower; }

    UFUNCTION(BlueprintPure, Category = "Stat")
    float GetFinalDefense() const { return Defense + ItemDefense; }

    UFUNCTION(BlueprintPure, Category = "Stat")
    float GetFinalMoveSpeed() const { return MoveSpeed + ItemMoveSpeed; }

    UFUNCTION(BlueprintPure, Category = "Stat")
    float GetFinalAttackSpeed() const { return AttackSpeed + ItemAttackSpeed; }

    UFUNCTION(BlueprintPure, Category = "Stat")
    float GetFinalAttackRange() const { return AttackRange + ItemAttackRange; }

    UFUNCTION(BlueprintPure, Category = "Stat")
    float GetFinalSkillAmplification() const { return SkillAmplification + ItemSkillAmplification; }

    UFUNCTION(BlueprintPure, Category = "Stat")
    float GetFinalCooldownReduction() const { return CooldownReduction + ItemCooldownReduction; }

    UFUNCTION(BlueprintPure, Category = "Stat")
    float GetFinalBasicAttackBonus() const { return BasicAttackBonus + ItemBasicAttackBonus; }

    UFUNCTION(BlueprintPure, Category = "Stat")
    float GetFinalDamageReduction() const { return DamageReduction + ItemDamageReduction; }

    UFUNCTION(BlueprintPure, Category = "Stat")
    float GetFinalLifeSteal() const { return LifeSteal + ItemLifeSteal; }

    UFUNCTION(BlueprintPure, Category = "Stat")
    float GetFinalCriticalChance() const { return CriticalChance + ItemCriticalChance; }

    UFUNCTION(BlueprintPure, Category = "Stat")
    float GetFinalCriticalDamage() const { return CriticalDamage + ItemCriticalDamage; }

    // =================== 기존 Setter ========================
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

    // =================== 기존 Getter ========================
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