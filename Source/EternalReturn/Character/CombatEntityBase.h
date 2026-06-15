#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ERTypes.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Stat/BaseStatComponent.h"
#include "CombatEntityBase.generated.h"

UCLASS()
class ETERNALRETURN_API ACombatEntityBase : public ACharacter
{
    GENERATED_BODY()

public:
    ACombatEntityBase();

    // ─── 전투 대상 ──────────────────────────────────
    UPROPERTY(BlueprintReadWrite, Category = "Combat")
    TObjectPtr<AActor> TargetActor;

    // ─── 데미지 ─────────────────────────────────────
    virtual float TakeDamage(
        float DamageAmount,
        struct FDamageEvent const& DamageEvent,
        AController* EventInstigator,
        AActor* DamageCauser) override;

    // ─── 상태이상 ────────────────────────────────────
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void ApplyStatusEffect(EStatusEffect Effect);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void RemoveStatusEffect(EStatusEffect Effect);

    UFUNCTION(BlueprintPure, Category = "Combat")
    bool HasStatusEffect(EStatusEffect Effect) const;

    // ─── Setter (BP의 InitializeStats에서 그대로 호출) ──
    UFUNCTION(BlueprintCallable, Category = "Stats") virtual void SetMaxHP(float value);
    UFUNCTION(BlueprintCallable, Category = "Stats") virtual void SetCurrentHP(float value);
    UFUNCTION(BlueprintCallable, Category = "Stats") virtual void SetHPRegen(float value);
    UFUNCTION(BlueprintCallable, Category = "Stats") virtual void SetAttackPower(float value);
    UFUNCTION(BlueprintCallable, Category = "Stats") virtual void SetDefense(float value);
    UFUNCTION(BlueprintCallable, Category = "Stats") virtual void SetMoveSpeed(float value);
    UFUNCTION(BlueprintCallable, Category = "Stats") virtual void SetAttackSpeed(float value);
    UFUNCTION(BlueprintCallable, Category = "Stats") virtual void SetAttackRange(float value);

    // ─── Getter ─────────────────────────────────────
    UFUNCTION(BlueprintPure, Category = "Stats") float GetCurrentHP() const;
    UFUNCTION(BlueprintPure, Category = "Stats") float GetMaxHP()     const;
    UFUNCTION(BlueprintPure, Category = "Stats") bool  IsDead()       const;

    // ─── StatComponent 델리게이트 핸들러 ────────────
    UFUNCTION()
    void HandleHPChanged(float InCurrentHP, float InMaxHP);

    UFUNCTION()
    void HandleDeath();
protected:
    // ─── StatComponent ───────────────────────────────
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
    TObjectPtr<UBaseStatComponent> StatComponent;

    // ─── 컴포넌트 ───────────────────────────────────
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    TObjectPtr<USphereComponent> AttackRangeSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    TObjectPtr<UWidgetComponent> HPBarWidget;

    // ─── 상태 ───────────────────────────────────────
    UPROPERTY(ReplicatedUsing = OnRep_ActiveStatusEffects, BlueprintReadOnly, Category = "State")
    TArray<EStatusEffect> ActiveStatusEffects;

    // ─── 사망 처리 ──────────────────────────────────
    UFUNCTION(BlueprintNativeEvent, Category = "Combat")
    void OnDeath();
    virtual void OnDeath_Implementation();

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_Ragdoll();

    // ─── UI 콜백 ────────────────────────────────────
    UFUNCTION(BlueprintImplementableEvent, Category = "UI")
    void OnHPChanged(float InCurrentHP, float InMaxHP);

    

    // ─── 리플리케이션 콜백 ──────────────────────────
    UFUNCTION()
    void OnRep_ActiveStatusEffects();

    virtual void BeginPlay() override;

    virtual void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};