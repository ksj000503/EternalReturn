#include "EternalReturnPlayerController.h"
#include "GameFramework/Pawn.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "EternalReturn.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AEternalReturnPlayerController::AEternalReturnPlayerController()
{
    PathFollowingComponent = CreateDefaultSubobject<UPathFollowingComponent>(TEXT("PathFollowingComponent"));

    bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Default;
    CachedDestination = FVector::ZeroVector;
    FollowTime = 0.f;

    PrimaryActorTick.bCanEverTick = true;
}

void AEternalReturnPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsFollowingPath) return;
    if (CurrentPath.Num() == 0) return;

    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn) return;

    FVector CurrentLocation = ControlledPawn->GetActorLocation();
    FVector NextPoint = CurrentPath[CurrentPathIndex];
    NextPoint.Z = CurrentLocation.Z;

    FVector Direction = (NextPoint - CurrentLocation).GetSafeNormal();
    float Distance = FVector::Dist2D(CurrentLocation, NextPoint);

    // AddMovementInput 으로 이동 → 회전, 애니메이션 자동 작동
    ControlledPawn->AddMovementInput(Direction, 1.f);

    // 웨이포인트 도달 시 다음으로
    if (Distance < AcceptanceRadius)
    {
        CurrentPathIndex++;
        if (CurrentPathIndex >= CurrentPath.Num())
        {
            bIsFollowingPath = false;
            CurrentPath.Empty();
        }
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

        if (FXCursor)
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, Destination,
                FRotator::ZeroRotator, FVector(1.f), true, true, ENCPoolMethod::None, true);
        }
    }
}

void AEternalReturnPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (IsLocalPlayerController())
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }

        if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
        {
            EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &AEternalReturnPlayerController::OnInputStarted);
            EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &AEternalReturnPlayerController::OnSetDestinationTriggered);
            EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &AEternalReturnPlayerController::OnSetDestinationReleased);
            EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &AEternalReturnPlayerController::OnSetDestinationReleased);
        }
    }
}

void AEternalReturnPlayerController::OnInputStarted()
{
    UpdateCachedDestination();
}

void AEternalReturnPlayerController::OnSetDestinationTriggered()
{
    FollowTime += GetWorld()->GetDeltaSeconds();
    UpdateCachedDestination();
}

void AEternalReturnPlayerController::OnSetDestinationReleased()
{
    RequestMoveTo(CachedDestination);
    FollowTime = 0.f;
}

void AEternalReturnPlayerController::UpdateCachedDestination()
{
    FHitResult Hit;
    if (GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit))
    {
        CachedDestination = Hit.Location;
    }
}