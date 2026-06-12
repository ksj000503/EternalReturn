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
#include "Structure/StructureBase.h"
#include "EternalReturn/Character/CraftingComponent.h"
#include "EternalReturn/Character/EternalReturnCharacter.h"
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

void AEternalReturnPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!HasAuthority() && !IsLocalController()) return;
    if (!bIsFollowingPath || CurrentPath.Num() == 0) return;

    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn) return;

    FVector CurrentLocation = ControlledPawn->GetActorLocation();
    FVector NextPoint = CurrentPath[CurrentPathIndex];
    NextPoint.Z = CurrentLocation.Z;

    FVector Direction = (NextPoint - CurrentLocation).GetSafeNormal();
    ControlledPawn->AddMovementInput(Direction, 1.f);

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

    Client_StopPathFollowing();
}

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

    if (NavPath && NavPath->IsValid() && NavPath->PathPoints.Num() >= 1)
    {
        CurrentPath = NavPath->PathPoints;
        CurrentPathIndex = 1;
        bIsFollowingPath = true;

        if (HasAuthority())
        {
            Client_StartPathFollowing(CurrentPath);
        }

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

void AEternalReturnPlayerController::AcknowledgePossession(APawn* P)
{
    Super::AcknowledgePossession(P);
    OnAcknowledgePossession(P);
}

// ─── Server RPC 구현 ────────────────────────────────

void AEternalReturnPlayerController::Server_RequestMoveTo_Implementation(FVector Destination)
{
    RequestMoveTo(Destination);
}

void AEternalReturnPlayerController::Client_StartPathFollowing_Implementation(const TArray<FVector>& Path)
{
    CurrentPath = Path;
    CurrentPathIndex = 1;
    bIsFollowingPath = true;
}

// ─── 크래프팅 Client RPC 구현 ───────────────────────

void AEternalReturnPlayerController::Client_OnCraftingStarted_Implementation(float CraftingTime)
{
    // 클라이언트에서 게이지 UI 표시
    OnCraftingStartedBP(CraftingTime);
}

void AEternalReturnPlayerController::Client_OnCraftingCancelled_Implementation()
{
    // 클라이언트에서 게이지 UI 숨기기
    OnCraftingCancelledBP();
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
        EIC->BindAction(CraftItemAction, ETriggerEvent::Started, this, &AEternalReturnPlayerController::OnCraftItem);
        EIC->BindAction(CameraLockAction, ETriggerEvent::Started, this, &AEternalReturnPlayerController::OnCameraLockStarted);
        EIC->BindAction(CameraLockAction, ETriggerEvent::Completed, this, &AEternalReturnPlayerController::OnCameraLockReleased);
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
    UpdateCachedDestination();
}

void AEternalReturnPlayerController::OnSetDestinationTriggered()
{
    if (TargetActor != nullptr) return;
    UpdateCachedDestination();
}

void AEternalReturnPlayerController::OnSetDestinationReleased()
{
    if (TargetActor == nullptr)
    {
        APawn* ControlledPawn = GetPawn();
        if (ControlledPawn)
        {
            AEternalReturnCharacter* ERCharacter = Cast<AEternalReturnCharacter>(ControlledPawn);
            if (ERCharacter && ERCharacter->CraftingComponent)
            {
                UE_LOG(LogTemp, Warning, TEXT("[OnSetDestinationReleased] bIsCrafting: %s"),
                    ERCharacter->CraftingComponent->bIsCrafting ? TEXT("true") : TEXT("false"));
                UE_LOG(LogTemp, Warning, TEXT("[OnSetDestinationReleased] Character Owner: %s"),
                    ERCharacter->GetOwner() ? *ERCharacter->GetOwner()->GetName() : TEXT("NULL"));

                if (ERCharacter->CraftingComponent->bIsCrafting)
                {
                    ERCharacter->CraftingComponent->CancelCrafting();
                    UE_LOG(LogTemp, Warning, TEXT("[OnSetDestinationReleased] CancelCrafting called"));
                }
            }
        }
        Server_RequestMoveTo(CachedDestination);
    }
}

void AEternalReturnPlayerController::OnCraftItem()
{
    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn) return;

    AEternalReturnCharacter* ERCharacter = Cast<AEternalReturnCharacter>(ControlledPawn);
    if (!ERCharacter) return;

    if (!ERCharacter->CraftingComponent) return;

    // 제작 가능한 아이템 없으면 무시
    if (ERCharacter->CraftingComponent->CraftableList.Num() == 0) return;

    // 이미 제작 중이면 무시
    if (ERCharacter->CraftingComponent->bIsCrafting) return;

    ERCharacter->CraftingComponent->StartCrafting(ERCharacter->CraftingComponent->CraftableList[0]);
}

// ─── 목적지 및 타겟 감지 ────────────────────────────

void AEternalReturnPlayerController::UpdateCachedDestination()
{
    FHitResult Hit;
    if (!GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit)) return;

    CachedDestination = Hit.Location;

    if (AActor* HitActor = Hit.GetActor())
    {
        if (HitActor != GetPawn() && HitActor->IsA<ACombatEntityBase>())
        {
            TargetActor = HitActor;
            CachedDestination = HitActor->GetActorLocation();
            OnEnemyClicked(HitActor);
            return;
        }
        if (HitActor->IsA<AStructureBase>())
        {
            CachedDestination = HitActor->GetActorLocation();
            OnStructureClicked(HitActor);
            return;
        }
    }

    TargetActor = nullptr;
    OnGroundClicked();
}