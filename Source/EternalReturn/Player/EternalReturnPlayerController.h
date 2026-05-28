#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EternalReturnPlayerController.generated.h"

class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 * 플레이어 입력 처리 및 캐릭터 이동 관리
 * NavMesh 경로 계산 + AddMovementInput 방식으로 이동
 * 적 클릭 감지 후 OnEnemyClicked 이벤트 호출
 */
UCLASS(abstract)
class ETERNALRETURN_API AEternalReturnPlayerController : public APlayerController
{
    GENERATED_BODY()

protected:
    // ─── 입력 에셋 ──────────────────────────────────

    /** 클릭 위치 이펙트 */
    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UNiagaraSystem> FXCursor;

    /** Enhanced Input 매핑 컨텍스트 */
    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputMappingContext> DefaultMappingContext;

    /** 이동 클릭 Input Action */
    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> SetDestinationClickAction;

    // ─── 이동 데이터 ────────────────────────────────

    /** 마지막으로 클릭한 목적지 */
    FVector CachedDestination;

    /** NavMesh로 계산된 경로 웨이포인트 목록 */
    TArray<FVector> CurrentPath;

    /** 현재 이동 중인 웨이포인트 인덱스 */
    int32 CurrentPathIndex = 0;

    /** 경로 이동 중 여부 */
    bool bIsFollowingPath = false;

    /** 웨이포인트 도달 판정 거리 */
    float AcceptanceRadius = 50.f;

public:
    AEternalReturnPlayerController();

    virtual void Tick(float DeltaTime) override;

    // ─── 전투 ───────────────────────────────────────

    /** 적 클릭 시 Blueprint에서 처리 (AttackTarget RPC 호출 등) */
    UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
    void OnEnemyClicked(AActor* EnemyActor);

    /** 땅 클릭 시 Blueprint에서 처리 (공격 중단 등) */
    UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
    void OnGroundClicked();

    /** 경로 이동 강제 중단 */
    UFUNCTION(BlueprintCallable, Category = "Movement")
    void StopPathFollowing();

    /** NavMesh로 경로 계산 후 이동 시작 */
    UFUNCTION(BlueprintCallable, Category = "Movement")
    void RequestMoveTo(FVector Destination);

    /** 특정 액터를 향해 이동 (추격) */
    UFUNCTION(BlueprintCallable, Category = "Movement")
    void FollowTarget(AActor* Target);

    /** 현재 공격 대상 */
    UPROPERTY(BlueprintReadWrite, Category = "Combat")
    TObjectPtr<AActor> TargetActor;

protected:
    virtual void SetupInputComponent() override;

    // ─── 입력 핸들러 ────────────────────────────────

    void OnInputStarted();
    void OnSetDestinationTriggered();
    void OnSetDestinationReleased();

    // ─── 이동 유틸 ──────────────────────────────────

    /** 커서 아래 위치 업데이트. 적 감지 시 OnEnemyClicked 호출 */
    void UpdateCachedDestination();
};