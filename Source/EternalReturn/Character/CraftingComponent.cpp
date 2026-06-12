#include "CraftingComponent.h"
#include "Net/UnrealNetwork.h"
#include "EternalReturnCharacter.h"
#include "InventoryComponent.h"
#include "EternalReturn/Data/ERTypes.h"
#include "EternalReturn/Player/EternalReturnPlayerController.h"

UCraftingComponent::UCraftingComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
    bIsCrafting = false;
}

void UCraftingComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UCraftingComponent, CraftableList);
    DOREPLIFETIME(UCraftingComponent, bIsCrafting); // 이동 시 취소 체크를 위해 복제
}

AEternalReturnPlayerController* UCraftingComponent::GetPlayerController() const
{
    AEternalReturnCharacter* Character = Cast<AEternalReturnCharacter>(GetOwner());
    if (!Character) return nullptr;

    return Cast<AEternalReturnPlayerController>(Character->GetController());
}

void UCraftingComponent::UpdateCraftableList_Implementation()
{
    AEternalReturnCharacter* Character = Cast<AEternalReturnCharacter>(GetOwner());
    if (!Character) return;

    UInventoryComponent* Inventory = Character->InventoryComponent;
    if (!Inventory) return;

    // 인벤토리 아이템 목록 추출
    TArray<FName> OwnedItems;
    for (const FInventorySlot& Slot : Inventory->InventorySlots)
    {
        if (Slot.ItemID != NAME_None)
        {
            OwnedItems.Add(Slot.ItemID);
        }
    }

    // DT_ItemData 순회하며 제작 가능 아이템 찾기
    CraftableList.Empty();
    if (!ItemDataTable) return;

    TArray<FName> RowNames = ItemDataTable->GetRowNames();
    for (const FName& RowName : RowNames)
    {
        FS_ItemData* Row = ItemDataTable->FindRow<FS_ItemData>(RowName, TEXT(""));
        if (!Row) continue;
        if (Row->Material1 == NAME_None || Row->Material2 == NAME_None) continue;
        if (OwnedItems.Contains(Row->Material1) && OwnedItems.Contains(Row->Material2))
        {
            CraftableList.Add(RowName);
        }
    }

    // 서버에서 직접 Broadcast (OnRep은 클라이언트에서만 발동)
    OnCraftableListUpdated.Broadcast();
}

void UCraftingComponent::StartCrafting_Implementation(FName RowName)
{
    // 이미 제작 중이면 무시
    if (bIsCrafting) return;

    if (!ItemDataTable) return;

    FS_ItemData* Row = ItemDataTable->FindRow<FS_ItemData>(RowName, TEXT(""));
    if (!Row) return;

    // 등급별 제작 시간 결정
    float CraftingTime = 0.f;
    switch (Row->ItemGrade)
    {
    case EItemGrade::Common:       CraftingTime = 2.0f; break;
    case EItemGrade::Uncommon:     CraftingTime = 3.0f; break;
    case EItemGrade::Rare:         CraftingTime = 4.0f; break;
    case EItemGrade::Epic:         CraftingTime = 5.0f; break;
    case EItemGrade::Legend:       CraftingTime = 6.0f; break;
    case EItemGrade::Transcendent: CraftingTime = 7.0f; break;
    default:                       CraftingTime = 2.0f; break;
    }

    bIsCrafting = true;
    CurrentCraftingItem = RowName;

    // 클라이언트에게 제작 시작 알림 (게이지 UI 표시)
    AEternalReturnPlayerController* PC = GetPlayerController();
    if (PC)
    {
        PC->Client_OnCraftingStarted(CraftingTime);
    }

    // 타이머 시작
    GetWorld()->GetTimerManager().SetTimer(
        CraftingTimerHandle,
        this,
        &UCraftingComponent::FinishCrafting,
        CraftingTime,
        false
    );
}

void UCraftingComponent::FinishCrafting()
{
    bIsCrafting = false;

    AEternalReturnCharacter* Character = Cast<AEternalReturnCharacter>(GetOwner());
    if (!Character) return;

    UInventoryComponent* Inventory = Character->InventoryComponent;
    if (!Inventory) return;

    if (!ItemDataTable) return;

    FS_ItemData* Row = ItemDataTable->FindRow<FS_ItemData>(CurrentCraftingItem, TEXT(""));
    if (!Row) return;

    // 재료 제거 후 결과물 추가
    Inventory->RemoveItem(Row->Material1);
    Inventory->RemoveItem(Row->Material2);
    Inventory->AddItem(CurrentCraftingItem);

    CurrentCraftingItem = NAME_None;

    // 제작 완료 알림 (서버 측)
    OnCraftingCompleted.Broadcast();
}

void UCraftingComponent::CancelCrafting_Implementation()
{
    if (!bIsCrafting) return;

    // 타이머 취소
    GetWorld()->GetTimerManager().ClearTimer(CraftingTimerHandle);

    bIsCrafting = false;
    CurrentCraftingItem = NAME_None;

    // 클라이언트에게 제작 취소 알림 (게이지 UI 숨기기)
    AEternalReturnPlayerController* PC = GetPlayerController();
    if (PC)
    {
        PC->Client_OnCraftingCancelled();
    }

    // 제작 취소 알림 (서버 측)
    OnCraftingCancelled.Broadcast();
}

void UCraftingComponent::OnRep_CraftableList()
{
    // 클라이언트에서 복제 완료 시 UI 갱신
    OnCraftableListUpdated.Broadcast();
}

void UCraftingComponent::OnRep_bIsCrafting()
{
    // 현재 비어있음 - 추후 클라이언트 측 추가 처리 필요 시 여기에 작성
}