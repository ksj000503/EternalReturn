// Fill out your copyright notice in the Description page of Project Settings.


#include "CraftingComponent.h"
#include "Net/UnrealNetwork.h"
#include "EternalReturn/Character/EternalReturnCharacter.h"
#include "EternalReturn/Character/InventoryComponent.h"
#include "EternalReturn/Data/ERTypes.h"

// Sets default values for this component's properties


UCraftingComponent::UCraftingComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UCraftingComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UCraftingComponent, CraftableList);
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
    // DataTable은 BP에서 설정할 수 있도록 UPROPERTY로 노출
    CraftableList.Empty();
    

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
}

void UCraftingComponent::CrateItem_Implementation()
{
    if (CraftableList.Num() == 0) return;

    AEternalReturnCharacter* Character = Cast<AEternalReturnCharacter>(GetOwner());
    if (!Character) return;

    UInventoryComponent* Inventory = Character->InventoryComponent;
    if (!Inventory) return;

    FName ItemToCraft = CraftableList[0];
    if (!ItemDataTable) return;

    FS_ItemData* Row = ItemDataTable->FindRow<FS_ItemData>(ItemToCraft, TEXT(""));
    if (!Row) return;

    Inventory->RemoveItem(Row->Material1);
    Inventory->RemoveItem(Row->Material2);
    Inventory->AddItem(ItemToCraft);
}

void UCraftingComponent::OnRep_CraftableList()
{
    OnCraftableListUpdated.Broadcast();
}