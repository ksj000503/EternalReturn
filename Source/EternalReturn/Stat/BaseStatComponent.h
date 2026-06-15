#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "BaseStatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHPChanged, float, CurrentHP, float, MaxHP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ETERNALRETURN_API UBaseStatComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UBaseStatComponent();
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
    UPROPERTY(BlueprintAssignable, Category = "Stat")
    FOnHPChanged OnHPChanged;
    UPROPERTY(BlueprintAssignable, Category = "Stat")
    FOnDeath OnDeath;

protected:
    UPROPERTY(ReplicatedUsing = OnRep_CurrentHP, VisibleAnywhere, Category = "Stat")
    float CurrentHP = 0.f;
    UPROPERTY(Replicated, VisibleAnywhere, Category = "Stat")
    float MaxHP = 0.f;
    UPROPERTY(Replicated, VisibleAnywhere, Category = "Stat")
    float HPRegen = 0.f;
    UPROPERTY(Replicated, VisibleAnywhere, Category = "Stat")
    float AttackPower = 0.f;
    UPROPERTY(Replicated, VisibleAnywhere, Category = "Stat")
    float Defense = 0.f;
    UPROPERTY(Replicated, VisibleAnywhere, Category = "Stat")
    float MoveSpeed = 0.f;
    UPROPERTY(Replicated, VisibleAnywhere, Category = "Stat")
    float AttackSpeed = 0.f;
    UPROPERTY(ReplicatedUsing = OnRep_AttackRange, VisibleAnywhere, Category = "Stat")
    float AttackRange = 150.f;
    UPROPERTY(ReplicatedUsing = OnRep_bIsDead, VisibleAnywhere, Category = "Stat")
    bool bIsDead = false;

private:
    UFUNCTION()
    void OnRep_CurrentHP();
    UFUNCTION()
    void OnRep_AttackRange();
    UFUNCTION()
    void OnRep_bIsDead();

public:
    UFUNCTION(BlueprintCallable, Category = "Stat")
    void SetMaxHP(float Value);
    UFUNCTION(BlueprintCallable, Category = "Stat")
    void SetCurrentHP(float Value);
    UFUNCTION(BlueprintCallable, Category = "Stat")
    void SetHPRegen(float Value);
    UFUNCTION(BlueprintCallable, Category = "Stat")
    void SetAttackPower(float Value);
    UFUNCTION(BlueprintCallable, Category = "Stat")
    void SetDefense(float Value);
    UFUNCTION(BlueprintCallable, Category = "Stat")
    void SetMoveSpeed(float Value);
    UFUNCTION(BlueprintCallable, Category = "Stat")
    void SetAttackSpeed(float Value);
    UFUNCTION(BlueprintCallable, Category = "Stat")
    void SetAttackRange(float Value);

    UFUNCTION(BlueprintPure, Category = "Stat")
    float GetMaxHP() const { return MaxHP; }
    UFUNCTION(BlueprintPure, Category = "Stat")
    float GetCurrentHP() const { return CurrentHP; }
    UFUNCTION(BlueprintPure, Category = "Stat")
    float GetAttackPower() const { return AttackPower; }
    UFUNCTION(BlueprintPure, Category = "Stat")
    float GetDefense() const { return Defense; }
    UFUNCTION(BlueprintPure, Category = "Stat")
    float GetMoveSpeed() const { return MoveSpeed; }
    UFUNCTION(BlueprintPure, Category = "Stat")
    float GetAttackSpeed() const { return AttackSpeed; }
    UFUNCTION(BlueprintPure, Category = "Stat")
    float GetAttackRange() const { return AttackRange; }

    UFUNCTION(BlueprintCallable, Category = "Stat")
    void ApplyDamage(float DamageAmount);
    UFUNCTION(BlueprintCallable, Category = "Stat")
    void Heal(float HealAmount);
    UFUNCTION(BlueprintCallable, Category = "Stat")
    bool IsAlive() const;
};