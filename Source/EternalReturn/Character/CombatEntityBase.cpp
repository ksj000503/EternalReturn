#include "CombatEntityBase.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

ACombatEntityBase::ACombatEntityBase()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    // ─── 기본 스탯 초기값 ────────────────────────────

    MaxHP = 100.f;
    CurrentHP = 100.f;
    HPRegen = 1.f;
    AttackPower = 10.f;
    Defense = 0.f;
    MoveSpeed = 350.f;
    AttackSpeed = 1.f;
    AttackRange = 150.f;
    bIsDead = false;

    // 커서 클릭 감지를 위해 캡슐에 Visibility 채널 Block 설정
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    // ─── 공격 사거리 콜라이더 ────────────────────────

    AttackRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttackRangeSphere"));
    AttackRangeSphere->SetupAttachment(RootComponent);
    AttackRangeSphere->SetSphereRadius(AttackRange);
    AttackRangeSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    AttackRangeSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    AttackRangeSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

// ─── 리플리케이션 등록 ───────────────────────────────

void ACombatEntityBase::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ACombatEntityBase, MaxHP);
    DOREPLIFETIME(ACombatEntityBase, CurrentHP);
    DOREPLIFETIME(ACombatEntityBase, HPRegen);
    DOREPLIFETIME(ACombatEntityBase, AttackPower);
    DOREPLIFETIME(ACombatEntityBase, Defense);
    DOREPLIFETIME(ACombatEntityBase, MoveSpeed);
    DOREPLIFETIME(ACombatEntityBase, AttackSpeed);
    DOREPLIFETIME(ACombatEntityBase, AttackRange);
    DOREPLIFETIME(ACombatEntityBase, bIsDead);
    DOREPLIFETIME(ACombatEntityBase, ActiveStatusEffects);
}

// ─── Setter (서버 전용) ──────────────────────────────

void ACombatEntityBase::SetMaxHP(float value)
{
    if (!HasAuthority()) return;
    MaxHP = value;
}

void ACombatEntityBase::SetHPRegen(float value)
{
    if (!HasAuthority()) return;
    HPRegen = value;
}

void ACombatEntityBase::SetAttackPower(float value)
{
    if (!HasAuthority()) return;
    AttackPower = value;
}

void ACombatEntityBase::SetDefense(float value)
{
    if (!HasAuthority()) return;
    Defense = value;
}

void ACombatEntityBase::SetMoveSpeed(float value)
{
    if (!HasAuthority()) return;
    MoveSpeed = value;
    GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
}

void ACombatEntityBase::SetAttackSpeed(float value)
{
    if (!HasAuthority()) return;
    AttackSpeed = value;
}

void ACombatEntityBase::SetAttackRange(float value)
{
    if (!HasAuthority()) return;
    AttackRange = value;
    if (AttackRangeSphere)
    {
        AttackRangeSphere->SetSphereRadius(value);
    }
}

// ─── 데미지 처리 ────────────────────────────────────

float ACombatEntityBase::TakeDamage(float DamageAmount,
    FDamageEvent const& DamageEvent,
    AController* EventInstigator,
    AActor* DamageCauser)
{
    if (!HasAuthority()) return 0.f;
    if (bIsDead)         return 0.f;

    // 몬스터는 공격받으면 공격자를 타겟으로 설정
    if (!IsPlayerControlled())
    {
        TargetActor = DamageCauser;
    }

    // 방어력 공식: 실제 피해 = 데미지 * 100 / (100 + 방어력)
    float ActualDamage = DamageAmount * (100.f / (100.f + Defense));
    ActualDamage = FMath::Max(1.f, ActualDamage);

    CurrentHP = FMath::Max(0.f, CurrentHP - ActualDamage);

    if (CurrentHP <= 0.f)
    {
        bIsDead = true;
        OnDeath();
    }

    return ActualDamage;
}

// ─── 상태이상 ────────────────────────────────────────

void ACombatEntityBase::ApplyStatusEffect(EStatusEffect Effect)
{
    if (!HasAuthority()) return;
    if (Effect == EStatusEffect::None) return;

    if (!ActiveStatusEffects.Contains(Effect))
    {
        ActiveStatusEffects.Add(Effect);
    }
}

void ACombatEntityBase::RemoveStatusEffect(EStatusEffect Effect)
{
    if (!HasAuthority()) return;
    ActiveStatusEffects.Remove(Effect);
}

bool ACombatEntityBase::HasStatusEffect(EStatusEffect Effect) const
{
    return ActiveStatusEffects.Contains(Effect);
}

// ─── 리플리케이션 콜백 ──────────────────────────────

void ACombatEntityBase::OnRep_CurrentHP()
{
    // HP 변경 시 클라이언트 처리 (HP바 UI 업데이트 등 - 추후 추가)
}

void ACombatEntityBase::OnRep_AttackRange()
{
    // 클라이언트 Sphere 반지름 동기화
    if (AttackRangeSphere)
    {
        AttackRangeSphere->SetSphereRadius(AttackRange);
    }
}

void ACombatEntityBase::OnRep_IsDead()
{
    if (bIsDead)
    {
        OnDeath();
    }
}

void ACombatEntityBase::OnRep_ActiveStatusEffects()
{
    // 상태이상 변경 시 클라이언트 처리 (UI, 이펙트 등 - 추후 추가)
}

// ─── 사망 처리 ──────────────────────────────────────

void ACombatEntityBase::OnDeath_Implementation()
{
    // 각 BP에서 재정의하여 사망 애니메이션, 이펙트 처리
}