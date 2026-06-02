#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ERTypes.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "CombatEntityBase.generated.h"

/**
 * 플레이어 캐릭터와 몬스터의 공통 베이스 클래스
 * HP, 스탯, 데미지, 상태이상, 사망 처리 담당
 *
 * [네트워크 구조]
 * - Dedicated Server 기준으로 설계
 * - 스탯 변수는 서버에서만 수정, 클라이언트에 리플리케이션
 * - TakeDamage, OnDeath는 서버에서만 실행
 * - TargetActor는 서버 전용 (플레이어: BP에서 Set, 몬스터: TakeDamage에서 Set)
 *
 * [블루프린트 확장]
 * - InitializeStats : BP에서 DataTable 읽어서 Setter 호출
 * - OnDeath         : BlueprintNativeEvent → BP에서 사망 연출 재정의
 * - TargetActor     : BlueprintReadWrite → BP의 AttackTarget, ClearTarget에서 직접 Set
 * - AttackRangeSphere: BP의 BeginOverlap / EndOverlap에 바인딩
 */
UCLASS()
class ETERNALRETURN_API ACombatEntityBase : public ACharacter
{
    GENERATED_BODY()

public:
    ACombatEntityBase();

    // ─── 전투 대상 ──────────────────────────────────
    // 플레이어: BP_Character의 AttackTarget/ClearTarget에서 Set
    // 몬스터  : TakeDamage에서 공격자로 자동 Set
    // Replicated 없음 → 서버 전용 변수 (클라이언트는 알 필요 없음)
    UPROPERTY(BlueprintReadWrite, Category = "Combat")
    TObjectPtr<AActor> TargetActor;

    // ─── 데미지 ─────────────────────────────────────
    // UE 기본 데미지 시스템 오버라이드. HasAuthority 체크로 서버에서만 처리
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

    // ─── Setter (서버 전용, HasAuthority 내부 체크) ──
    // BP의 InitializeStats 함수에서 DataTable 값을 받아 호출
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
    // ─── 스탯 ───────────────────────────────────────
    // BlueprintReadOnly: BP에서 읽기만 가능, 수정은 Setter 통해서만
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
    float MaxHP;

    // OnRep_CurrentHP: HP 변경 시 클라이언트에서 UI 업데이트 등에 활용
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

    // OnRep_AttackRange: AttackRange 변경 시 클라이언트 Sphere 반지름도 동기화
    UPROPERTY(ReplicatedUsing = OnRep_AttackRange, BlueprintReadOnly, Category = "Stats")
    float AttackRange;

    // ─── 컴포넌트 ───────────────────────────────────
    // BP의 BeginOverlap/EndOverlap 이벤트가 이 컴포넌트에 바인딩됨
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    TObjectPtr<USphereComponent> AttackRangeSphere;

    // HP바 위젯 컴포넌트 (머리 위 World Space)
    // BP_Character, BP_MonsterBase 모두 공통 사용
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    TObjectPtr<UWidgetComponent> HPBarWidget;

    // ─── 상태 ───────────────────────────────────────
    // OnRep_IsDead: 클라이언트에서 사망 연출 처리
    UPROPERTY(ReplicatedUsing = OnRep_IsDead, BlueprintReadOnly, Category = "State")
    bool bIsDead;

    UPROPERTY(ReplicatedUsing = OnRep_ActiveStatusEffects, BlueprintReadOnly, Category = "State")
    TArray<EStatusEffect> ActiveStatusEffects;

    // ─── 사망 처리 ──────────────────────────────────
    // BlueprintNativeEvent: C++ 기본 구현 제공, BP에서 재정의 가능
    // BP_Character, BP_MonsterBase에서 각각 사망 애니메이션/이펙트 처리
    UFUNCTION(BlueprintNativeEvent, Category = "Combat")
    void OnDeath();
    virtual void OnDeath_Implementation();

    // ─── 리플리케이션 콜백 ──────────────────────────
    UFUNCTION() virtual void OnRep_CurrentHP();
    UFUNCTION()         void OnRep_AttackRange();
    UFUNCTION() virtual void OnRep_IsDead();
    UFUNCTION()         void OnRep_ActiveStatusEffects();

    virtual void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};