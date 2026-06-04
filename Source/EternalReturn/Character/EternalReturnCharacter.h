#pragma once

#include "CoreMinimal.h"
#include "CombatEntityBase.h"
#include "InventoryComponent.h"

#include "EternalReturnCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

/**
 * 플레이어 캐릭터 베이스 클래스
 * 카메라, 캐릭터 전용 스탯, 재화, 상태 관리
 *
 * [네트워크 구조]
 * - Dedicated Server 기준
 * - 카메라는 로컬 클라이언트에만 존재 (서버에서는 의미 없음)
 * - 스탯/재화는 서버에서 수정, 클라이언트에 리플리케이션
 *
 * [블루프린트 확장]
 * - BP_Character    : 기본 공격 로직 (AttackTarget, TryAttack 등)
 * - BP_Subject_{Name}: 캐릭터별 스킬 구현 (QWERDF)
 * - InitializeStats : DataTable에서 스탯 읽어서 Setter 호출
 *
 * [스킬 구조 - 추후 구현]
 * - QWERDF 입력 시 각각 BlueprintImplementableEvent 호출
 * - BP_Subject_{Name}에서 캐릭터별로 재정의
 */
UCLASS(abstract)
class ETERNALRETURN_API AEternalReturnCharacter : public ACombatEntityBase
{
    GENERATED_BODY()

public:
    AEternalReturnCharacter();

    // ─── Getter ─────────────────────────────────────
    UFUNCTION(BlueprintPure, Category = "State")
    bool IsResting() const { return bIsResting; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    EWeaponType AllowedWeaponType = EWeaponType::None;

    UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent.Get(); }
    USpringArmComponent* GetCameraBoom()             const { return CameraBoom.Get(); }

protected:
    // ─── 카메라 ─────────────────────────────────────
    // 탑다운 카메라 (로컬 클라이언트 전용)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UCameraComponent> TopDownCameraComponent;

    // 카메라 거리/각도 조절용 스프링암
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    TObjectPtr<UInventoryComponent> InventoryComponent;

    // ─── 캐릭터 전용 스탯 ───────────────────────────
    // CombatEntityBase의 기본 스탯 외 캐릭터 전용 추가 스탯
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
    float SkillAmplification;   // 스킬 증폭

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
    float CooldownReduction;    // 쿨다운 감소

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
    float BasicAttackBonus;     // 기본 공격 추가 데미지

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
    float DamageReduction;      // 피해 감소율

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
    float CriticalChance;       // 치명타 확률

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
    float CriticalDamage;       // 치명타 피해율 (기본 150%)

    // ─── 재화 ───────────────────────────────────────
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