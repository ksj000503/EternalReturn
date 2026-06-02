#include "CombatEntityBase.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"

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
    // UpdateCachedDestination의 GetHitResultUnderCursor가 이 채널을 사용함
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    // ─── 공격 사거리 콜라이더 ────────────────────────
    // BP의 BeginOverlap/EndOverlap에 바인딩됨
    // SetAttackRange 호출 시 반지름이 업데이트됨
    AttackRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttackRangeSphere"));
    AttackRangeSphere->SetupAttachment(RootComponent);
    AttackRangeSphere->SetSphereRadius(AttackRange);
    AttackRangeSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    AttackRangeSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    AttackRangeSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    // ─── HP바 위젯 컴포넌트 ──────────────────────────
    // Widget Class는 BP_Character, BP_MonsterBase에서 각각 설정
    HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBarWidget"));
    HPBarWidget->SetupAttachment(RootComponent);
    HPBarWidget->SetWidgetSpace(EWidgetSpace::World);
    HPBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
    HPBarWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
    // TargetActor는 서버 전용이므로 리플리케이션 등록 안 함
}

// ─── Setter (서버 전용) ──────────────────────────────
// BP의 InitializeStats 함수에서 DataTable 값을 받아 호출
// HasAuthority 체크로 클라이언트에서 호출되어도 무시됨

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

float ACombatEntityBase::TakeDamage(
    float DamageAmount,
    FDamageEvent const& DamageEvent,
    AController* EventInstigator,
    AActor* DamageCauser)
{
    // Dedicated Server에서만 처리
    if (!HasAuthority()) return 0.f;
    if (bIsDead)         return 0.f;

    // 몬스터는 공격받으면 공격자를 TargetActor로 설정
    // BTS_MonsterService가 이 값을 Blackboard에 동기화
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
    // 클라이언트: HP 변경 시 HP바 UI 업데이트 처리
    // BP_Character에서 OnRep_CurrentHP를 오버라이드하여 구현
}

void ACombatEntityBase::OnRep_AttackRange()
{
    // 클라이언트: AttackRange 변경 시 Sphere 반지름 동기화
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
    // 클라이언트: 상태이상 변경 시 UI, 이펙트 처리
}

// ─── 사망 처리 ──────────────────────────────────────

void ACombatEntityBase::OnDeath_Implementation()
{
    // BP_Character, BP_MonsterBase에서 재정의하여
    // 사망 애니메이션, 이펙트, 사망 후 처리 구현
}