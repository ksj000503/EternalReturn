#include "InventoryComponent.h"
#include "EternalReturnCharacter.h"
#include "Net/UnrealNetwork.h"
#include "CraftingComponent.h"
#include "Stat/BaseStatComponent.h"
#include "Stat/CharacterStatComponent.h" // 추가

UInventoryComponent::UInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UInventoryComponent::BeginPlay()
{
    Super::BeginPlay();

    if (GetOwner()->HasAuthority())
    {
        for (int i = 0; i < 10; i++)
        {
            FInventorySlot Slot;
            Slot.bIsEmpty = true;
            InventorySlots.Add(Slot);
        }

        for (int i = 0; i < 5; i++)
        {
            FInventorySlot Slot;
            Slot.bIsEmpty = true;
            EquipSlots.Add(Slot);
        }
    }
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UInventoryComponent::OnRep_InventorySlots()
{
    OnInventoryUpdated.Broadcast();
}

void UInventoryComponent::OnRep_EquipSlots()
{
    OnEquipSlotsUpdated.Broadcast();
}

UCraftingComponent* UInventoryComponent::GetCraftingComponent()
{
    return Cast<UCraftingComponent>(GetOwner()->GetComponentByClass(UCraftingComponent::StaticClass()));
}

bool UInventoryComponent::AddItem(FName ItemID)
{

    if (!GetOwner()->HasAuthority())
    {
        return false;
    }

    for (int i = 0; i < InventorySlots.Num(); i++)
    {
        if (InventorySlots[i].bIsEmpty)
        {
            InventorySlots[i].ItemID = ItemID;
            InventorySlots[i].bIsEmpty = false;

            UCraftingComponent* Crafting = GetCraftingComponent();
            if (Crafting)
            {
                Crafting->UpdateCraftableList();
            }

            // 서버(호스트) 자신은 OnRep_InventorySlots가 자동 호출되지 않으므로 수동 브로드캐스트
            OnInventoryUpdated.Broadcast();

            return true;
        }
    }

    return false;
}

bool UInventoryComponent::RemoveItem(FName ItemID)
{
    if (!GetOwner()->HasAuthority())
    {
        return false;
    }

    for (int i = 0; i < InventorySlots.Num(); i++)
    {
        if (InventorySlots[i].ItemID == ItemID)
        {
            InventorySlots[i].bIsEmpty = true;
            InventorySlots[i].ItemID = NAME_None;

            UCraftingComponent* Crafting = GetCraftingComponent();
            if (Crafting)
            {
                Crafting->UpdateCraftableList();
            }

            // 서버(호스트) 자신은 OnRep_InventorySlots가 자동 호출되지 않으므로 수동 브로드캐스트
            OnInventoryUpdated.Broadcast();

            return true;
        }
    }

    return false;
}

bool UInventoryComponent::EquipItem(int32 InventoryIndex)
{
    if (!GetOwner()->HasAuthority())
    {
        return false;
    }

    if (!InventorySlots.IsValidIndex(InventoryIndex))
    {
        return false;
    }

    if (InventorySlots[InventoryIndex].bIsEmpty)
    {
        return false;
    }

    FName ItemID = InventorySlots[InventoryIndex].ItemID;

    FS_ItemData* ItemData = ItemDataTable->FindRow<FS_ItemData>(ItemID, TEXT(""));
    if (ItemData == nullptr)
    {
        return false;
    }

    int32 SlotIndex = -1;

    switch (ItemData->ItemType)
    {
    case EItemType::Weapon:
    {
        AEternalReturnCharacter* Character = Cast<AEternalReturnCharacter>(GetOwner());
        if (Character && ItemData->WeaponType != Character->AllowedWeaponType)
        {
            return false;
        }
        SlotIndex = 0;
        break;
    }
    case EItemType::Chest: SlotIndex = 1; break;
    case EItemType::Head:  SlotIndex = 2; break;
    case EItemType::Arm:   SlotIndex = 3; break;
    case EItemType::Leg:   SlotIndex = 4; break;
    }

    if (SlotIndex == -1)
    {
        return false;
    }

    // 장비 슬롯에 이미 아이템이 있으면 기존 스탯 제거 후 인벤토리로 돌려보냄 (추가)
    if (!EquipSlots[SlotIndex].bIsEmpty)
    {
        FS_ItemData* OldItemData = ItemDataTable->FindRow<FS_ItemData>(EquipSlots[SlotIndex].ItemID, TEXT(""));
        AEternalReturnCharacter* Character = Cast<AEternalReturnCharacter>(GetOwner());
        if (OldItemData && Character && Character->CharacterStatComponent)
        {
            Character->CharacterStatComponent->RemoveItemStats(OldItemData->Stats);
        }
        AddItem(EquipSlots[SlotIndex].ItemID);
    }

    // 장비 슬롯에 아이템 장착
    EquipSlots[SlotIndex].ItemID = ItemID;
    EquipSlots[SlotIndex].bIsEmpty = false;

    // 인벤토리 슬롯 비우기
    InventorySlots[InventoryIndex].ItemID = NAME_None;
    InventorySlots[InventoryIndex].bIsEmpty = true;

    // 아이템 스탯 적용 (추가)
    AEternalReturnCharacter* Character = Cast<AEternalReturnCharacter>(GetOwner());
    if (Character && Character->CharacterStatComponent)
    {
        Character->CharacterStatComponent->ApplyItemStats(ItemData->Stats);
    }

    OnInventoryUpdated.Broadcast();
    OnEquipSlotsUpdated.Broadcast();

    return true;
}

bool UInventoryComponent::UnequipItem(int32 EquipSlotIndex)
{
    if (!GetOwner()->HasAuthority())
    {
        return false;
    }

    if (!EquipSlots.IsValidIndex(EquipSlotIndex))
    {
        return false;
    }

    if (EquipSlots[EquipSlotIndex].bIsEmpty)
    {
        return false;
    }

    // 아이템 스탯 제거 (추가)
    FS_ItemData* ItemData = ItemDataTable->FindRow<FS_ItemData>(EquipSlots[EquipSlotIndex].ItemID, TEXT(""));
    AEternalReturnCharacter* Character = Cast<AEternalReturnCharacter>(GetOwner());
    if (ItemData && Character && Character->CharacterStatComponent)
    {
        Character->CharacterStatComponent->RemoveItemStats(ItemData->Stats);
    }

    // 인벤토리에 공간 없으면 해제 불가
    if (!AddItem(EquipSlots[EquipSlotIndex].ItemID))
    {
        return false;
    }

    // 장비 슬롯 비우기
    EquipSlots[EquipSlotIndex].ItemID = NAME_None;
    EquipSlots[EquipSlotIndex].bIsEmpty = true;

    // 서버(호스트) 자신은 OnRep_EquipSlots가 자동 호출되지 않으므로 수동 브로드캐스트
    // (인벤토리 쪽은 위에서 호출한 AddItem() 내부의 Broadcast로 이미 갱신됨)
    OnEquipSlotsUpdated.Broadcast();

    return true;
}

void UInventoryComponent::UseItem_Implementation(int32 SlotIndex)
{
    if (!GetOwner()->HasAuthority())
    {
        return;
    }

    if (!InventorySlots.IsValidIndex(SlotIndex))
    {
        return;
    }

    if (InventorySlots[SlotIndex].bIsEmpty)
    {
        return;
    }

    FName ItemID = InventorySlots[SlotIndex].ItemID;

    FS_ItemData* ItemData = ItemDataTable->FindRow<FS_ItemData>(ItemID, TEXT(""));

    if (!ItemData)
    {
        return;
    }

    switch (ItemData->ItemType)
    {
    case EItemType::Weapon:
    case EItemType::Chest:
    case EItemType::Head:
    case EItemType::Arm:
    case EItemType::Leg:
        EquipItem(SlotIndex);
        break;

    case EItemType::Food:
    {
        UBaseStatComponent* StatComponent = Cast<UBaseStatComponent>(GetOwner()->GetComponentByClass(UBaseStatComponent::StaticClass()));

        if (StatComponent)
        {
            StatComponent->Heal(ItemData->HPRestore);
            InventorySlots[SlotIndex].ItemID = NAME_None;
            InventorySlots[SlotIndex].bIsEmpty = true;
            OnInventoryUpdated.Broadcast();
        }
        break;
    }
    case EItemType::Material:
        return;
    }
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UInventoryComponent, InventorySlots);
    DOREPLIFETIME(UInventoryComponent, EquipSlots);
}