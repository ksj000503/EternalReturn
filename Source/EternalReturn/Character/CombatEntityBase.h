// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ERTypes.h"
#include "Components/SphereComponent.h"
#include "CombatEntityBase.generated.h"

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
    float AttackRange;;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    TObjectPtr<USphereComponent> AttackRangeSphere;

    // ─── 상태 ───────────────────────────────────────

    UPROPERTY(ReplicatedUsing = OnRep_IsDead, BlueprintReadOnly, Category = "State")
    bool bIsDead;

    UPROPERTY(ReplicatedUsing = OnRep_ActiveStatusEffects, BlueprintReadOnly, Category = "State")
    TArray<EStatusEffect> ActiveStatusEffects;

    // ─── 리플리케이션 콜백 ──────────────────────────

    UFUNCTION()
    virtual void OnRep_CurrentHP();

    UFUNCTION()
    void OnRep_AttackRange();

    UFUNCTION()
    virtual void OnRep_IsDead();

    UFUNCTION()
    void OnRep_ActiveStatusEffects();

    // ─── 사망 처리 ──────────────────────────────────

    UFUNCTION(BlueprintNativeEvent, Category = "Combat")
    void OnDeath();
    virtual void OnDeath_Implementation();

public:
    // ─── 데미지 ─────────────────────────────────────

    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
        AController* EventInstigator, AActor* DamageCauser) override;

    // ─── 상태이상 ────────────────────────────────────

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void ApplyStatusEffect(EStatusEffect Effect);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void RemoveStatusEffect(EStatusEffect Effect);

    UFUNCTION(BlueprintPure, Category = "Combat")
    bool HasStatusEffect(EStatusEffect Effect) const;

    // ─── Getter ─────────────────────────────────────

    UFUNCTION(BlueprintPure, Category = "Stats")
    float GetCurrentHP() const { return CurrentHP; }

    UFUNCTION(BlueprintPure, Category = "Stats")
    float GetMaxHP() const { return MaxHP; }

    UFUNCTION(BlueprintPure, Category = "Stats")
    bool IsDead() const { return bIsDead; }

    UPROPERTY(BlueprintReadWrite, Category = "Combat")
    TObjectPtr<AActor> TargetActor;

protected:
    virtual void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
    UFUNCTION(BlueprintCallable)
    void SetMaxHP(float value);

    UFUNCTION(BlueprintCallable)
    void SetHPRegen(float value);

    UFUNCTION(BlueprintCallable)
    void SetAttackPower(float value);

    UFUNCTION(BlueprintCallable)
    void SetDefense(float value);

    UFUNCTION(BlueprintCallable)
    void SetMoveSpeed(float value);

    UFUNCTION(BlueprintCallable)
    void SetAttackSpeed(float value);

    UFUNCTION(BlueprintCallable)
    void SetAttackRange(float value);
};