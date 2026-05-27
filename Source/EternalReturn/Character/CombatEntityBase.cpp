#include "CombatEntityBase.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"


ACombatEntityBase::ACombatEntityBase()
{
    PrimaryActorTick.bCanEverTick = true;

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

    AttackRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttackRangeSphere"));
    AttackRangeSphere->SetupAttachment(RootComponent);
    AttackRangeSphere->SetSphereRadius(AttackRange);
    AttackRangeSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    AttackRangeSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    AttackRangeSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ACombatEntityBase::BeginPlay()
{
    Super::BeginPlay();
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
    DOREPLIFETIME(ACombatEntityBase, ActiveStatusEffects);  // ← 추가
}

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
    if (bIsDead) return 0.f;

    if (!IsPlayerControlled())
    {
        TargetActor = DamageCauser;
    }

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

    // 중복 적용 방지
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
    // 클라이언트 HP 변경 시 처리
    // 예: HP 바 UI 업데이트 → 나중에 추가
}

void ACombatEntityBase::OnRep_AttackRange()
{
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
    // 클라이언트에서 상태이상 변경 시 처리
    // 예: 상태이상 아이콘 UI, 이펙트 → 나중에 추가
}

// ─── 사망 처리 ──────────────────────────────────────

void ACombatEntityBase::OnDeath_Implementation()
{
    // 각 BP에서 재정의
    // 사망 애니메이션, 이펙트 등 추가
}