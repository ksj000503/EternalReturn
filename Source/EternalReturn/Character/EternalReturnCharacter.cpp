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

    // 이동 방향으로 캐릭터 자동 회전 (NavMesh 기반 클릭 이동에 필요)
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);

    // ─── 캐릭터 전용 스탯 초기값 ─────────────────────
    SkillAmplification = 0.f;
    CooldownReduction = 0.f;
    BasicAttackBonus = 0.f;
    DamageReduction = 0.f;
    CriticalChance = 0.f;
    CriticalDamage = 150.f; // 이터널 리턴 기본값

    // ─── 재화 초기값 ─────────────────────────────────
    Gold = 0;
    Experience = 0;

    // ─── 상태 초기값 ─────────────────────────────────
    bIsResting = false;
}

// ─── 리플리케이션 등록 ───────────────────────────────

void AEternalReturnCharacter::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AEternalReturnCharacter, SkillAmplification);
    DOREPLIFETIME(AEternalReturnCharacter, CooldownReduction);
    DOREPLIFETIME(AEternalReturnCharacter, BasicAttackBonus);
    DOREPLIFETIME(AEternalReturnCharacter, DamageReduction);
    DOREPLIFETIME(AEternalReturnCharacter, CriticalChance);
    DOREPLIFETIME(AEternalReturnCharacter, CriticalDamage);
    DOREPLIFETIME(AEternalReturnCharacter, Gold);
    DOREPLIFETIME(AEternalReturnCharacter, Experience);
    DOREPLIFETIME(AEternalReturnCharacter, bIsResting);
}

// ─── 리플리케이션 콜백 ──────────────────────────────

void AEternalReturnCharacter::OnRep_IsResting()
{
    // 클라이언트: 휴식 상태 변경 시 애니메이션 등 처리
    // 추후 휴식 애니메이션 구현 시 여기에 추가
}