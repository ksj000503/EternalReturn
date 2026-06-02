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
 */
UCLASS(abstract)
class ETERNALRETURN_API AEternalReturnPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AEternalReturnPlayerController();

    virtual void Tick(float DeltaTime) override;

    // ─── 전투 이벤트 (BP에서 구현) ──────────────────
    // 적 클릭 시 BP에서 GetPawn → Cast BP_Character → AttackTarget 호출
    UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
    void OnEnemyClicked(AActor* EnemyActor);

    // 땅 클릭 시 BP에서 GetPawn → Cast BP_Character → ClearTarget 호출
    UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
    void OnGroundClicked();

    // ─── 이동 함수 ──────────────────────────────────
    // 서버에서 경로 이동 강제 중단 + Client RPC로 클라이언트도 정지
    // BP_Character의 TryAttack에서 StopPathFollowing 호출 후 공격 시작
    UFUNCTION(BlueprintCallable, Category = "Movement")
    void StopPathFollowing();

    // NavMesh로 경로 계산 후 이동 시작 (서버에서만 유효)
    UFUNCTION(BlueprintCallable, Category = "Movement")
    void RequestMoveTo(FVector Destination);

    // 특정 액터를 향해 이동 (EndOverlap 시 자동 추격)
    UFUNCTION(BlueprintCallable, Category = "Movement")
    void FollowTarget(AActor* Target);

    // ─── Server RPC ─────────────────────────────────
    // 클라이언트 클릭 → 서버에서 경로 계산 및 이동 시작
    UFUNCTION(Server, Reliable)
    void Server_RequestMoveTo(FVector Destination);

    // ─── 현재 공격 대상 ─────────────────────────────
    // UpdateCachedDestination에서 적 감지 시 Set
    // OnSetDestinationTriggered에서 드래그 이동 무시 판단에 사용
    UPROPERTY(BlueprintReadWrite, Category = "Combat")
    TObjectPtr<AActor> TargetActor;

protected:
    // ─── 입력 에셋 ──────────────────────────────────
    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UNiagaraSystem> FXCursor;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputMappingContext> DefaultMappingContext;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> SetDestinationClickAction;

    // ─── 이동 데이터 (서버 전용) ────────────────────
    // Dedicated Server에서 PlayerController는 서버 + 해당 클라이언트에만 존재
    // 경로 계산은 서버에서, 클라이언트 Tick에서 AddMovementInput 실행
    // → 두 인스턴스가 각자 자신의 데이터를 가지므로 Replicated 불필요
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

    // ─── 이동 유틸 ──────────────────────────────────
    // 커서 아래 위치 업데이트
    // 적 감지 시 OnEnemyClicked, 땅 클릭 시 OnGroundClicked 호출
    void UpdateCachedDestination();

private:
    // ─── Client RPC ─────────────────────────────────
    // 서버에서 StopPathFollowing 호출 시 클라이언트에게도 정지 명령 전달
    // Dedicated Server: 서버 Tick만 멈추면 클라이언트 Tick은 계속 이동함
    // → 이 RPC로 클라이언트 bIsFollowingPath = false 처리
    UFUNCTION(Client, Reliable)
    void Client_StopPathFollowing();

    // 서버에서 계산한 경로 포인트 배열을 클라이언트에 직접 전달
    // 클라이언트에 NavMesh가 없어도 서버 경로 그대로 사용 가능
    UFUNCTION(Client, Reliable)
    void Client_StartPathFollowing(const TArray<FVector>& Path);
};