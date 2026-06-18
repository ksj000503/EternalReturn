#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"

#include "EternalReturnPlayerController.generated.h"

class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 * 플레이어 입력 처리 및 캐릭터 이동 관리
 * NavMesh 경로 계산 + AddMovementInput 방식으로 이동
 *
 * [네트워크 구조 - Dedicated Server 기준]
 * - PlayerController는 서버 + 해당 클라이언트에만 존재
 * - 입력은 클라이언트에서 감지 → Server RPC로 서버에 전달
 * - 이동 경로 계산은 서버에서만 수행
 * - bIsFollowingPath, CurrentPath는 서버 전용 (Replicated 불필요)
 * - StopPathFollowing은 서버 + 클라이언트 둘 다 실행해야 즉시 정지됨
 *   → Client_StopPathFollowing RPC로 클라이언트에게 정지 명령 전달
 *
 * [블루프린트 연결]
 * - OnEnemyClicked → BP_PlayerController에서 GetPawn → Cast → AttackTarget 호출
 * - OnGroundClicked → BP_PlayerController에서 GetPawn → Cast → ClearTarget 호출
 * - OnCraftingStartedBP → BP_PlayerController에서 WBP_CraftingGauge StartGauge 호출
 * - OnCraftingCancelledBP → BP_PlayerController에서 WBP_CraftingGauge Set Visibility Hidden 호출
 */
UCLASS(abstract)
class ETERNALRETURN_API AEternalReturnPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AEternalReturnPlayerController();

    virtual void Tick(float DeltaTime) override;

    // ─── 전투 이벤트 (BP에서 구현) ──────────────────
    UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
    void OnEnemyClicked(AActor* EnemyActor);

    UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
    void OnGroundClicked();

    // ─── 이동 함수 ──────────────────────────────────
    UFUNCTION(BlueprintCallable, Category = "Movement")
    void StopPathFollowing();

    UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
    void OnStructureClicked(AActor* StructureActor);

    UFUNCTION(BlueprintCallable, Category = "Movement")
    void RequestMoveTo(FVector Destination);

    UFUNCTION(BlueprintCallable, Category = "Movement")
    void FollowTarget(AActor* Target);

    // ─── Server RPC ─────────────────────────────────
    UFUNCTION(Server, Reliable)
    void Server_RequestMoveTo(FVector Destination);

    // ─── 현재 공격 대상 ─────────────────────────────
    UPROPERTY(BlueprintReadWrite, Category = "Combat")
    TObjectPtr<AActor> TargetActor;

    // ─── 크래프팅 이벤트 (BP에서 구현) ──────────────
    // 서버에서 제작 시작 시 클라이언트에게 알림 → WBP_CraftingGauge StartGauge 호출
    UFUNCTION(BlueprintImplementableEvent, Category = "Crafting")
    void OnCraftingStartedBP(float CraftingTime);

    // 서버에서 제작 취소 시 클라이언트에게 알림 → WBP_CraftingGauge Set Visibility Hidden 호출
    UFUNCTION(BlueprintImplementableEvent, Category = "Crafting")
    void OnCraftingCancelledBP();


    // 서버에서 제작 시작 시 클라이언트에게 전달
    UFUNCTION(Client, Reliable)
    void Client_OnCraftingStarted(float CraftingTime);

    // 서버에서 제작 취소 시 클라이언트에게 전달
    UFUNCTION(Client, Reliable)
    void Client_OnCraftingCancelled();
protected:

    virtual void AcknowledgePossession(class APawn* P) override;

    UFUNCTION(BlueprintImplementableEvent)
    void OnAcknowledgePossession(class APawn* P);

    // ─── 입력 에셋 ──────────────────────────────────
    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UNiagaraSystem> FXCursor;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputMappingContext> DefaultMappingContext;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> SetDestinationClickAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> CraftItemAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> CameraLockAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> UseInventoryAction;

    // 스킬 입력
    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> SkillQAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> SkillWAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> SkillEAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> SkillRAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> SkillDAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> SkillFAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> StopAction;


    // ─── 이동 데이터 ────────────────────────────────
    FVector          CachedDestination;
    TArray<FVector>  CurrentPath;
    int32            CurrentPathIndex = 0;
    bool             bIsFollowingPath = false;
    float            AcceptanceRadius = 50.f;

    virtual void SetupInputComponent() override;

    // ─── 입력 핸들러 ────────────────────────────────
    void OnInputStarted();
    void OnSetDestinationTriggered();
    void OnSetDestinationReleased();
    void OnCraftItem();
    void OnUseInventory(const FInputActionInstance& Instance);

    UFUNCTION(BlueprintImplementableEvent)
    void OnCameraLockStarted();

    UFUNCTION(BlueprintImplementableEvent)
    void OnCameraLockReleased();

    void UpdateCachedDestination();

    UFUNCTION(BlueprintImplementableEvent, Category = "Input")
    void OnSkillQ();

    UFUNCTION(BlueprintImplementableEvent, Category = "Input")
    void OnSkillW();

    UFUNCTION(BlueprintImplementableEvent, Category = "Input")
    void OnSkillE();

    UFUNCTION(BlueprintImplementableEvent, Category = "Input")
    void OnSkillR();

    UFUNCTION(BlueprintImplementableEvent, Category = "Input")
    void OnSkillD();

    UFUNCTION(BlueprintImplementableEvent, Category = "Input")
    void OnSkillF();

    UFUNCTION(BlueprintImplementableEvent, Category = "Input")
    void OnStop();
private:
    // ─── Client RPC ─────────────────────────────────
    UFUNCTION(Client, Reliable)
    void Client_StopPathFollowing();

    UFUNCTION(Client, Reliable)
    void Client_StartPathFollowing(const TArray<FVector>& Path);

    UFUNCTION(Server, Reliable)
    void Server_UseItem(int32 SlotIndex);
};