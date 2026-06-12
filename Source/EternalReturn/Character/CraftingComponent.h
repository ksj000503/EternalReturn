#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CraftingComponent.generated.h"

class AEternalReturnPlayerController;

// 제작 가능 목록 갱신 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCraftableListUpdated);
// 제작 시작 델리게이트 (제작 시간 포함)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCraftingStarted, float, CraftingTime);
// 제작 완료 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCraftingCompleted);
// 제작 취소 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCraftingCancelled);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ETERNALRETURN_API UCraftingComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCraftingComponent();

    // 제작 가능 목록 (클라이언트에 복제)
    UPROPERTY(ReplicatedUsing = OnRep_CraftableList, BlueprintReadOnly)
    TArray<FName> CraftableList;

    // 현재 제작 중인 아이템
    UPROPERTY(BlueprintReadOnly)
    FName CurrentCraftingItem;

    // 제작 중 여부 (클라이언트에 복제 - 이동 시 취소 체크에 사용)
    UPROPERTY(ReplicatedUsing = OnRep_bIsCrafting, BlueprintReadOnly)
    bool bIsCrafting;

    // 인벤토리 변경 시 제작 가능 목록 갱신
    UFUNCTION(BlueprintCallable, Server, Reliable)
    void UpdateCraftableList();

    // 제작 시작
    UFUNCTION(BlueprintCallable, Server, Reliable)
    void StartCrafting(FName RowName);

    // 제작 취소
    UFUNCTION(BlueprintCallable, Server, Reliable)
    void CancelCrafting();

    // 데이터 테이블
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
    TObjectPtr<UDataTable> ItemDataTable;

    // 델리게이트
    UPROPERTY(BlueprintAssignable)
    FOnCraftableListUpdated OnCraftableListUpdated;

    UPROPERTY(BlueprintAssignable)
    FOnCraftingStarted OnCraftingStarted;

    UPROPERTY(BlueprintAssignable)
    FOnCraftingCompleted OnCraftingCompleted;

    UPROPERTY(BlueprintAssignable)
    FOnCraftingCancelled OnCraftingCancelled;

protected:
    // 타이머 핸들
    FTimerHandle CraftingTimerHandle;

    // 제작 완료 처리 (타이머 콜백)
    void FinishCrafting();

    // CraftableList 복제 완료 시 UI 갱신
    UFUNCTION()
    void OnRep_CraftableList();

    // bIsCrafting 복제 완료 시 호출 (현재 비어있음, 추후 확장 가능)
    UFUNCTION()
    void OnRep_bIsCrafting();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    // PlayerController 헬퍼
    AEternalReturnPlayerController* GetPlayerController() const;
};