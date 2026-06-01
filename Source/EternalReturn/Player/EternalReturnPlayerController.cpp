#include "EternalReturnPlayerController.h"
#include "GameFramework/Pawn.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "EternalReturn.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "CombatEntityBase.h"
#include "GameFramework/PawnMovementComponent.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AEternalReturnPlayerController::AEternalReturnPlayerController()
{
    bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Default;
    CachedDestination = FVector::ZeroVector;

    PrimaryActorTick.bCanEverTick = true;
}

// ─── 이동 처리 (Tick) ────────────────────────────────
// Dedicated Server 구조:
//   서버 인스턴스: 경로 계산 + AddMovementInput 실행 → 실제 캐릭터 이동
//   클라이언트 인스턴스: 서버와 동일한 경로를 가지고 AddMovementInput 실행
//   → 클라이언트 예측 이동 (서버 보정으로 최종 위치 동기화)

void AEternalReturnPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsFollowingPath || CurrentPath.Num() == 0) return;

    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn) return;

    FVector CurrentLocation = ControlledPawn->GetActorLocation();
    FVector NextPoint = CurrentPath[CurrentPathIndex];
    NextPoint.Z = CurrentLocation.Z; // Z축 고정 (탑다운 이동)

    // 다음 웨이포인트 방향으로 이동
    FVector Direction = (NextPoint - CurrentLocation).GetSafeNormal();
    ControlledPawn->AddMovementInput(Direction, 1.f);

    // 웨이포인트 도달 시 다음 포인트로 전환
    if (FVector::Dist2D(CurrentLocation, NextPoint) < AcceptanceRadius)
    {
        CurrentPathIndex++;
        if (CurrentPathIndex >= CurrentPath.Num())
        {
            bIsFollowingPath = false;
            CurrentPath.Empty();
        }
    }
}

// ─── 이동 중단 ──────────────────────────────────────

void AEternalReturnPlayerController::StopPathFollowing()
{
    bIsFollowingPath = false;
    CurrentPath.Empty();

    if (APawn* ControlledPawn = GetPawn())
    {
        ControlledPawn->GetMovementComponent()->StopMovementImmediately();
    }

    // 클라이언트에게도 이동 중단 명령 전달
    // Dedicated Server: 서버만 멈추면 클라이언트 Tick에서 계속 이동하는 문제 방지
    Client_StopPathFollowing();
}

// Client RPC 구현
// 클라이언트에서 실행되어 클라이언트의 bIsFollowingPath를 false로 변경
void AEternalReturnPlayerController::Client_StopPathFollowing_Implementation()
{
    bIsFollowingPath = false;
    CurrentPath.Empty();

    if (APawn* ControlledPawn = GetPawn())
    {
        ControlledPawn->GetMovementComponent()->StopMovementImmediately();
    }
}

// ─── 경로 이동 ──────────────────────────────────────

void AEternalReturnPlayerController::RequestMoveTo(FVector Destination)
{
    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn) return;

    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
    if (!NavSys) return;

    UNavigationPath* NavPath = NavSys->FindPathToLocationSynchronously(
        GetWorld(), ControlledPawn->GetActorLocation(), Destination);

    if (NavPath && NavPath->IsValid() && NavPath->PathPoints.Num() > 1)
    {
        CurrentPath = NavPath->PathPoints;
        CurrentPathIndex = 1;
        bIsFollowingPath = true;

        // 클릭 위치 이펙트 재생
        if (FXCursor)
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                this, FXCursor, Destination,
                FRotator::ZeroRotator, FVector(1.f),
                true, true, ENCPoolMethod::None, true);
        }
    }
}

void AEternalReturnPlayerController::FollowTarget(AActor* Target)
{
    if (!Target) return;
    RequestMoveTo(Target->GetActorLocation());
}

// ─── Server RPC 구현 ────────────────────────────────

void AEternalReturnPlayerController::Server_RequestMoveTo_Implementation(FVector Destination)
{
    RequestMoveTo(Destination);
}

// ─── 입력 세팅 ──────────────────────────────────────

void AEternalReturnPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // 로컬 클라이언트에서만 입력 바인딩
    if (!IsLocalPlayerController()) return;

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
        ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        Subsystem->AddMappingContext(DefaultMappingContext, 0);
    }

    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
    {
        EIC->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &AEternalReturnPlayerController::OnInputStarted);
        EIC->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &AEternalReturnPlayerController::OnSetDestinationTriggered);
        EIC->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &AEternalReturnPlayerController::OnSetDestinationReleased);
        EIC->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &AEternalReturnPlayerController::OnSetDestinationReleased);
    }
    else
    {
        UE_LOG(LogTemplateCharacter, Error,
            TEXT("'%s' Enhanced Input Component를 찾을 수 없습니다."), *GetNameSafe(this));
    }
}

// ─── 입력 핸들러 ────────────────────────────────────

void AEternalReturnPlayerController::OnInputStarted()
{
    // 클릭 시작: 적/땅 감지 (1회)
    UpdateCachedDestination();
}

void AEternalReturnPlayerController::OnSetDestinationTriggered()
{
    // 드래그 이동: 타겟 있으면 무시 (공격 중 드래그로 이동 취소 방지)
    if (TargetActor != nullptr) return;
    UpdateCachedDestination();
}

void AEternalReturnPlayerController::OnSetDestinationReleased()
{
    // 땅 클릭 해제: 타겟 없을 때만 이동 명령 전송
    if (TargetActor == nullptr)
    {
        Server_RequestMoveTo(CachedDestination);
    }
}

// ─── 목적지 및 타겟 감지 ────────────────────────────

void AEternalReturnPlayerController::UpdateCachedDestination()
{
    FHitResult Hit;
    if (!GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit)) return;

    CachedDestination = Hit.Location;

    // 적(CombatEntityBase) 클릭 감지
    // CombatEntityBase 생성자에서 Visibility = Block으로 설정했기 때문에 Hit 가능
    if (AActor* HitActor = Hit.GetActor())
    {
        if (HitActor != GetPawn() && HitActor->IsA<ACombatEntityBase>())
        {
            TargetActor = HitActor;
            CachedDestination = HitActor->GetActorLocation();

            // BP_PlayerController에서 GetPawn → Cast BP_Character → AttackTarget 호출
            OnEnemyClicked(HitActor);
            return;
        }
    }

    // 땅 클릭: 타겟 초기화 + BP에 알림
    // BP_PlayerController에서 GetPawn → Cast BP_Character → ClearTarget 호출
    TargetActor = nullptr;
    OnGroundClicked();
}