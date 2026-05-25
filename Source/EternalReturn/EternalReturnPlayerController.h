#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EternalReturnPlayerController.generated.h"

class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;
class UPathFollowingComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(abstract)
class AEternalReturnPlayerController : public APlayerController
{
    GENERATED_BODY()

protected:
    UPROPERTY(VisibleDefaultsOnly, Category = AI)
    TObjectPtr<UPathFollowingComponent> PathFollowingComponent;

    UPROPERTY(EditAnywhere, Category = "Input")
    float ShortPressThreshold;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UNiagaraSystem> FXCursor;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputMappingContext> DefaultMappingContext;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> SetDestinationClickAction;

    FVector CachedDestination;
    float FollowTime = 0.f;

    // ─── 경로 이동 ───────────────────────────────────
    TArray<FVector> CurrentPath;
    int32 CurrentPathIndex = 0;
    bool bIsFollowingPath = false;

    float AcceptanceRadius = 50.f;

public:
    AEternalReturnPlayerController();

    virtual void Tick(float DeltaTime) override;

protected:
    virtual void SetupInputComponent() override;

    void OnInputStarted();
    void OnSetDestinationTriggered();
    void OnSetDestinationReleased();
    void UpdateCachedDestination();

    void RequestMoveTo(FVector Destination);
};