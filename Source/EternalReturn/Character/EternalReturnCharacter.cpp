#include "EternalReturnCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AEternalReturnCharacter::AEternalReturnCharacter()
{
    // ─── 카메라 세팅 ─────────────────────────────────
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->SetUsingAbsoluteRotation(true);
    CameraBoom->TargetArmLength = 800.f;
    CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
    CameraBoom->bDoCollisionTest = false;

    TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
    TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    TopDownCameraComponent->bUsePawnControlRotation = false;

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);

    // ─── 컴포넌트 생성 ───────────────────────────────
    InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
    CharacterStatComponent = CreateDefaultSubobject<UCharacterStatComponent>(TEXT("CharacterStatComponent"));

    // ─── 재화 초기값 ─────────────────────────────────
    Gold = 0;
    Experience = 0;

    // ─── 상태 초기값 ─────────────────────────────────
    bIsResting = false;
}

void AEternalReturnCharacter::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AEternalReturnCharacter, Gold);
    DOREPLIFETIME(AEternalReturnCharacter, Experience);
    DOREPLIFETIME(AEternalReturnCharacter, bIsResting);
    // 캐릭터 전용 스탯은 CharacterStatComponent 내부에서 복제 처리
}

// ─── Setter (서버 전용, CharacterStatComponent로 리다이렉트) ──

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

// ─── Getter ──────────────────────────────────────────

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