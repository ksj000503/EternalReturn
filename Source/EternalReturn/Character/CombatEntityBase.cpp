#include "CombatEntityBase.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"

ACombatEntityBase::ACombatEntityBase()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    // ─── StatComponent 생성 ──────────────────────────
    StatComponent = CreateDefaultSubobject<UBaseStatComponent>(TEXT("StatComponent"));

    // ─── 공격 사거리 콜라이더 ────────────────────────
    AttackRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttackRangeSphere"));
    AttackRangeSphere->SetupAttachment(RootComponent);
    AttackRangeSphere->SetSphereRadius(150.f);
    AttackRangeSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    AttackRangeSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    AttackRangeSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    // ─── HP바 위젯 컴포넌트 ──────────────────────────
    HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBarWidget"));
    HPBarWidget->SetupAttachment(RootComponent);
    HPBarWidget->SetWidgetSpace(EWidgetSpace::World);
    HPBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
    HPBarWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ACombatEntityBase::BeginPlay()
{
    Super::BeginPlay();

    // ─── StatComponent 델리게이트 바인딩 ────────────
    // HP 변경 → OnHPChanged(BP 이벤트) 호출
    // 사망    → OnDeath 호출
    if (StatComponent)
    {
        StatComponent->OnHPChanged.AddDynamic(this, &ACombatEntityBase::HandleHPChanged);
        StatComponent->OnDeath.AddDynamic(this, &ACombatEntityBase::HandleDeath);
    }
}

void ACombatEntityBase::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ACombatEntityBase, ActiveStatusEffects);
    // 스탯 변수들은 StatComponent 내부에서 복제 처리
}

// ─── Setter (서버 전용, BP의 InitializeStats에서 호출) ──
// HasAuthority 체크 후 StatComponent로 리다이렉트

void ACombatEntityBase::SetMaxHP(float value)
{
    if (!HasAuthority()) return;
    StatComponent->SetMaxHP(value);
    StatComponent->SetCurrentHP(value); // MaxHP 설정 시 CurrentHP도 동기화
}

void ACombatEntityBase::SetCurrentHP(float value)
{
    if (!HasAuthority()) return;
    StatComponent->SetCurrentHP(value);
}

void ACombatEntityBase::SetHPRegen(float value)
{
    if (!HasAuthority()) return;
    StatComponent->SetHPRegen(value);
}

void ACombatEntityBase::SetAttackPower(float value)
{
    if (!HasAuthority()) return;
    StatComponent->SetAttackPower(value);
}

void ACombatEntityBase::SetDefense(float value)
{
    if (!HasAuthority()) return;
    StatComponent->SetDefense(value);
}

void ACombatEntityBase::SetMoveSpeed(float value)
{
    if (!HasAuthority()) return;
    StatComponent->SetMoveSpeed(value);
    // CharacterMovement 연동은 CombatEntityBase에서 직접 처리
    GetCharacterMovement()->MaxWalkSpeed = value;
}

void ACombatEntityBase::SetAttackSpeed(float value)
{
    if (!HasAuthority()) return;
    StatComponent->SetAttackSpeed(value);
}

void ACombatEntityBase::SetAttackRange(float value)
{
    if (!HasAuthority()) return;
    StatComponent->SetAttackRange(value);
    // AttackRangeSphere 연동은 CombatEntityBase에서 직접 처리
    if (AttackRangeSphere)
    {
        AttackRangeSphere->SetSphereRadius(value);
    }
}

// ─── Getter ──────────────────────────────────────────

float ACombatEntityBase::GetCurrentHP() const
{
    return StatComponent ? StatComponent->GetCurrentHP() : 0.f;
}

float ACombatEntityBase::GetMaxHP() const
{
    return StatComponent ? StatComponent->GetMaxHP() : 0.f;
}

bool ACombatEntityBase::IsDead() const
{
    return StatComponent ? !StatComponent->IsAlive() : true;
}

// ─── 데미지 처리 ────────────────────────────────────
// 기존 이터널리턴 방어력 공식 유지
// 실제 피해 = DamageAmount * 100 / (100 + Defense)

float ACombatEntityBase::TakeDamage(
    float DamageAmount,
    FDamageEvent const& DamageEvent,
    AController* EventInstigator,
    AActor* DamageCauser)
{
    if (!HasAuthority()) return 0.f;
    if (IsDead()) return 0.f;

    // 몬스터는 공격받으면 공격자를 TargetActor로 설정
    if (!IsPlayerControlled())
    {
        TargetActor = DamageCauser;
    }

    // 방어력 공식: 실제 피해 = 데미지 * 100 / (100 + 방어력)
    float Defense = StatComponent->GetDefense();
    float ActualDamage = DamageAmount * (100.f / (100.f + Defense));
    ActualDamage = FMath::Max(1.f, ActualDamage);

    StatComponent->SetCurrentHP(GetCurrentHP() - ActualDamage);

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

// ─── StatComponent 델리게이트 핸들러 ────────────────

void ACombatEntityBase::HandleHPChanged(float InCurrentHP, float InMaxHP)
{
    // StatComponent HP 변경 → BP의 OnHPChanged 이벤트 호출
    OnHPChanged(InCurrentHP, InMaxHP);
}

void ACombatEntityBase::HandleDeath()
{
    // StatComponent 사망 → OnDeath 호출
    OnDeath();
}

// ─── 리플리케이션 콜백 ──────────────────────────────

void ACombatEntityBase::OnRep_ActiveStatusEffects()
{
    // 클라이언트: 상태이상 변경 시 UI, 이펙트 처리
}

// ─── 사망 처리 ──────────────────────────────────────

void ACombatEntityBase::OnDeath_Implementation()
{
    Multicast_Ragdoll();
}

void ACombatEntityBase::Multicast_Ragdoll_Implementation()
{
    USkeletalMeshComponent* SkelMesh = GetMesh();
    if (SkelMesh)
    {
        SkelMesh->SetSimulatePhysics(true);
        SkelMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    }

    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetCharacterMovement()->DisableMovement();

    FTimerHandle DestroyTimer;
    GetWorldTimerManager().SetTimer(DestroyTimer, [this]()
        {
            SetActorHiddenInGame(true);
        }, 30.f, false);
}