#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ERTypes.h"
#include "Components/SphereComponent.h"
#include "CombatEntityBase.generated.h"

/**
 * 캐릭터와 몬스터의 공통 베이스 클래스
 * HP, 스탯, 데미지, 상태이상, 사망 처리 담당
 */
UCLASS()
class ETERNALRETURN_API ACombatEntityBase : public ACharacter
{
    GENERATED_BODY()

public:
    ACombatEntityBase();

protected:
    virtual void BeginPlay() override;

    // ─── 스탯 ───────────────────────────────────────

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
    float MaxHP;

    UPROPERTY(ReplicatedUsing = OnRep_CurrentHP, BlueprintReadOnly, Category = "Stats")
    float CurrentHP;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
    float HPRegen;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
    float AttackPower;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
    float Defense;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
    float MoveSpeed;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
    float AttackSpeed;

    UPROPERTY(ReplicatedUsing = OnRep_AttackRange, BlueprintReadOnly, Category = "Stats")
    float AttackRange;

    // ─── 컴포넌트 ───────────────────────────────────

    /** 기본 공격 사거리를 감지하는 구체 콜라이더 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    TObjectPtr<USphereComponent> AttackRangeSphere;

    // ─── 상태 ───────────────────────────────────────

    UPROPERTY(ReplicatedUsing = OnRep_IsDead, BlueprintReadOnly, Category = "State")
    bool bIsDead;

    UPROPERTY(ReplicatedUsing = OnRep_ActiveStatusEffects, BlueprintReadOnly, Category = "State")
    TArray<EStatusEffect> ActiveStatusEffects;

    // ─── 리플리케이션 콜백 ──────────────────────────

    UFUNCTION() virtual void OnRep_CurrentHP();
    UFUNCTION() void OnRep_AttackRange();
    UFUNCTION() virtual void OnRep_IsDead();
    UFUNCTION() void OnRep_ActiveStatusEffects();

    // ─── 사망 처리 ──────────────────────────────────

    /** BP에서 재정의하여 사망 연출 처리 */
    UFUNCTION(BlueprintNativeEvent, Category = "Combat")
    void OnDeath();
    virtual void OnDeath_Implementation();

public:
    // ─── 전투 대상 ──────────────────────────────────

    /** 현재 공격 중인 대상 */
    UPROPERTY(BlueprintReadWrite, Category = "Combat")
    TObjectPtr<AActor> TargetActor;

    // ─── 데미지 ─────────────────────────────────────

    /** UE 기본 데미지 시스템 오버라이드. 서버에서만 처리 */
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
        AController* EventInstigator, AActor* DamageCauser) override;

    // ─── 상태이상 ────────────────────────────────────

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void ApplyStatusEffect(EStatusEffect Effect);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void RemoveStatusEffect(EStatusEffect Effect);

    UFUNCTION(BlueprintPure, Category = "Combat")
    bool HasStatusEffect(EStatusEffect Effect) const;

    // ─── Setter (서버 전용) ──────────────────────────

    UFUNCTION(BlueprintCallable, Category = "Stats") void SetMaxHP(float value);
    UFUNCTION(BlueprintCallable, Category = "Stats") void SetHPRegen(float value);
    UFUNCTION(BlueprintCallable, Category = "Stats") void SetAttackPower(float value);
    UFUNCTION(BlueprintCallable, Category = "Stats") void SetDefense(float value);
    UFUNCTION(BlueprintCallable, Category = "Stats") void SetMoveSpeed(float value);
    UFUNCTION(BlueprintCallable, Category = "Stats") void SetAttackSpeed(float value);
    UFUNCTION(BlueprintCallable, Category = "Stats") void SetAttackRange(float value);

    // ─── Getter ─────────────────────────────────────

    UFUNCTION(BlueprintPure, Category = "Stats") float GetCurrentHP() const { return CurrentHP; }
    UFUNCTION(BlueprintPure, Category = "Stats") float GetMaxHP()     const { return MaxHP; }
    UFUNCTION(BlueprintPure, Category = "Stats") bool  IsDead()       const { return bIsDead; }

protected:
    virtual void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};