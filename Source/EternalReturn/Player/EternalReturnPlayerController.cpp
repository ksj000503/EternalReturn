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
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AEternalReturnPlayerController::AEternalReturnPlayerController()
{
    bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Default;
    CachedDestination = FVector::ZeroVector;

    PrimaryActorTick.bCanEverTick = true;
}

// ─── 이동 처리 (Tick) ────────────────────────────────

void AEternalReturnPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsFollowingPath || CurrentPath.Num() == 0) return;

    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn) return;

    FVector CurrentLocation = ControlledPawn->GetActorLocation();
    FVector NextPoint = CurrentPath[CurrentPathIndex];
    NextPoint.Z = CurrentLocation.Z;

    // 다음 웨이포인트 방향으로 이동 (회전/애니메이션 자동 작동)
    FVector Direction = (NextPoint - CurrentLocation).GetSafeNormal();
    ControlledPawn->AddMovementInput(Direction, 1.f);

    // 웨이포인트 도달 시 다음 포인트로 이동
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

// ─── 경로 이동 ──────────────────────────────────────

void AEternalReturnPlayerController::StopPathFollowing()
{
    bIsFollowingPath = false;
    CurrentPath.Empty();

    // 캐릭터 이동 즉시 정지
    if (APawn* ControlledPawn = GetPawn())
    {
        ControlledPawn->GetMovementComponent()->StopMovementImmediately();
    }
}

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
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, Destination,
                FRotator::ZeroRotator, FVector(1.f), true, true, ENCPoolMethod::None, true);
        }
    }
}


void AEternalReturnPlayerController::FollowTarget(AActor* Target)
{
    // 대상이 없으면 무시
    if (!Target) return;

    // 대상의 현재 위치로 이동 경로 계산
    RequestMoveTo(Target->GetActorLocation());
}

void AEternalReturnPlayerController::Server_RequestMoveTo_Implementation(FVector Destination)
{
    UE_LOG(LogTemplateCharacter, Warning, TEXT("Server_RequestMoveTo called"));
    RequestMoveTo(Destination);
}

// ─── 입력 세팅 ──────────────────────────────────────

void AEternalReturnPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

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
        UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Enhanced Input Component를 찾을 수 없습니다."), *GetNameSafe(this));
    }
}

// ─── 입력 핸들러 ────────────────────────────────────

void AEternalReturnPlayerController::OnInputStarted()
{
    // 클릭 시작 시 목적지/타겟 감지
    UpdateCachedDestination();
}

void AEternalReturnPlayerController::OnSetDestinationTriggered()
{
    // 타겟이 있으면 무시, 땅 드래그 이동만 처리
    if (TargetActor != nullptr) return;
    UpdateCachedDestination();
}

void AEternalReturnPlayerController::OnSetDestinationReleased()
{
    UE_LOG(LogTemplateCharacter, Warning, TEXT("Released - HasAuthority: %d, IsLocalController: %d"), HasAuthority(), IsLocalController());

    if (TargetActor == nullptr)
    {
        if (IsLocalController())
        {
            Server_RequestMoveTo(CachedDestination);
        }
    }
}

// ─── 목적지 업데이트 ────────────────────────────────

void AEternalReturnPlayerController::UpdateCachedDestination()
{
    FHitResult Hit;
    if (!GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit)) return;

    CachedDestination = Hit.Location;

    // 적(CombatEntityBase) 클릭 감지
    if (AActor* HitActor = Hit.GetActor())
    {
        if (HitActor != GetPawn() && HitActor->IsA<ACombatEntityBase>())
        {
            TargetActor = HitActor;
            CachedDestination = HitActor->GetActorLocation();
            OnEnemyClicked(HitActor);
            return;
        }
    }

    // 땅 클릭 시 타겟 초기화 및 BP에 알림
    TargetActor = nullptr;
    OnGroundClicked();
}

void AEternalReturnPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AEternalReturnPlayerController, CurrentPath);
    DOREPLIFETIME(AEternalReturnPlayerController, CurrentPathIndex);
    DOREPLIFETIME(AEternalReturnPlayerController, bIsFollowingPath);
}