#include "EternalReturnCharacter.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CraftingComponent.h"

AEternalReturnCharacter::AEternalReturnCharacter()
{
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);

    // ─── 컴포넌트 생성 ───────────────────────────────
    InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
    CharacterStatComponent = CreateDefaultSubobject<UCharacterStatComponent>(TEXT("CharacterStatComponent"));
    CraftingComponent = CreateDefaultSubobject<UCraftingComponent>(TEXT("CraftingComponent"));

    // ─── 재화 초기값 ─────────────────────────────────
    Gold = 0;
    Experience = 0;

    // ─── 상태 초기값 ─────────────────────────────────
    bIsResting = false;
}

void AEternalReturnCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (CharacterStatComponent)
    {
        CharacterStatComponent->OnHPChanged.AddDynamic(this, &ACombatEntityBase::HandleHPChanged);
        CharacterStatComponent->OnDeath.AddDynamic(this, &ACombatEntityBase::HandleDeath);
    }
}

void AEternalReturnCharacter::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AEternalReturnCharacter, Gold);
    DOREPLIFETIME(AEternalReturnCharacter, Experience);
    DOREPLIFETIME(AEternalReturnCharacter, bIsResting);
}

// ─── Setter 오버라이드 — CharacterStatComponent + StatComponent 동기화 ──

void AEternalReturnCharacter::SetMaxHP(float value)
{
    if (!HasAuthority()) return;
    CharacterStatComponent->SetMaxHP(value);
    CharacterStatComponent->SetCurrentHP(value);
    StatComponent->SetMaxHP(value);
    StatComponent->SetCurrentHP(value);
}

void AEternalReturnCharacter::SetCurrentHP(float value)
{
    if (!HasAuthority()) return;
    CharacterStatComponent->SetCurrentHP(value);
    StatComponent->SetCurrentHP(value);
}

void AEternalReturnCharacter::SetHPRegen(float value)
{
    if (!HasAuthority()) return;
    CharacterStatComponent->SetHPRegen(value);
    StatComponent->SetHPRegen(value);
}

void AEternalReturnCharacter::SetAttackPower(float value)
{
    if (!HasAuthority()) return;
    CharacterStatComponent->SetAttackPower(value);
    StatComponent->SetAttackPower(value);
}

void AEternalReturnCharacter::SetDefense(float value)
{
    if (!HasAuthority()) return;
    CharacterStatComponent->SetDefense(value);
    StatComponent->SetDefense(value);
}

void AEternalReturnCharacter::SetMoveSpeed(float value)
{
    if (!HasAuthority()) return;
    CharacterStatComponent->SetMoveSpeed(value);
    StatComponent->SetMoveSpeed(value);
    GetCharacterMovement()->MaxWalkSpeed = value;
}

void AEternalReturnCharacter::SetAttackSpeed(float value)
{
    if (!HasAuthority()) return;
    CharacterStatComponent->SetAttackSpeed(value);
    StatComponent->SetAttackSpeed(value);
}

void AEternalReturnCharacter::SetAttackRange(float value)
{
    if (!HasAuthority()) return;
    CharacterStatComponent->SetAttackRange(value);
    StatComponent->SetAttackRange(value);
    if (AttackRangeSphere)
    {
        AttackRangeSphere->SetSphereRadius(value);
    }
}

// ─── CharacterStatComponent 전용 Setter ──

void AEternalReturnCharacter::SetSkillAmplification(float Value)
{
    if (!HasAuthority()) return;
    CharacterStatComponent->SetSkillAmplification(Value);
}

void AEternalReturnCharacter::SetCooldownReduction(float Value)
{
    if (!HasAuthority()) return;
    CharacterStatComponent->SetCooldownReduction(Value);
}

void AEternalReturnCharacter::SetBasicAttackBonus(float Value)
{
    if (!HasAuthority()) return;
    CharacterStatComponent->SetBasicAttackBonus(Value);
}

void AEternalReturnCharacter::SetDamageReduction(float Value)
{
    if (!HasAuthority()) return;
    CharacterStatComponent->SetDamageReduction(Value);
}

void AEternalReturnCharacter::SetCriticalChance(float Value)
{
    if (!HasAuthority()) return;
    CharacterStatComponent->SetCriticalChance(Value);
}

void AEternalReturnCharacter::SetCriticalDamage(float Value)
{
    if (!HasAuthority()) return;
    CharacterStatComponent->SetCriticalDamage(Value);
}

void AEternalReturnCharacter::SetLifeSteal(float Value)
{
    if (!HasAuthority()) return;
    CharacterStatComponent->SetLifeSteal(Value);
}

void AEternalReturnCharacter::SetVisionRange(float Value)
{
    if (!HasAuthority()) return;
    CharacterStatComponent->SetVisionRange(Value);
}

// ─── Getter ──

float AEternalReturnCharacter::GetSkillAmplification() const
{
    return CharacterStatComponent ? CharacterStatComponent->GetSkillAmplification() : 0.f;
}

float AEternalReturnCharacter::GetCooldownReduction() const
{
    return CharacterStatComponent ? CharacterStatComponent->GetCooldownReduction() : 0.f;
}

float AEternalReturnCharacter::GetBasicAttackBonus() const
{
    return CharacterStatComponent ? CharacterStatComponent->GetBasicAttackBonus() : 0.f;
}

float AEternalReturnCharacter::GetDamageReduction() const
{
    return CharacterStatComponent ? CharacterStatComponent->GetDamageReduction() : 0.f;
}

float AEternalReturnCharacter::GetCriticalChance() const
{
    return CharacterStatComponent ? CharacterStatComponent->GetCriticalChance() : 0.f;
}

float AEternalReturnCharacter::GetCriticalDamage() const
{
    return CharacterStatComponent ? CharacterStatComponent->GetCriticalDamage() : 0.f;
}

float AEternalReturnCharacter::GetLifeSteal() const
{
    return CharacterStatComponent ? CharacterStatComponent->GetLifeSteal() : 0.f;
}

float AEternalReturnCharacter::GetVisionRange() const
{
    return CharacterStatComponent ? CharacterStatComponent->GetVisionRange() : 0.f;
}

// ─── 리플리케이션 콜백 ──────────────────────────────

void AEternalReturnCharacter::OnRep_IsResting()
{
    // 클라이언트: 휴식 상태 변경 시 애니메이션 등 처리
}