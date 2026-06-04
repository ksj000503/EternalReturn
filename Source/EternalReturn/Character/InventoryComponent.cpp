#include "InventoryComponent.h"
#include "EternalReturnCharacter.h"
#include "Net/UnrealNetwork.h"

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
    UE_LOG(LogTemp, Warning, TEXT("OnRep_InventorySlots called"));
    OnInventoryUpdated.Broadcast();
}

void UInventoryComponent::OnRep_EquipSlots()
{
    UE_LOG(LogTemp, Warning, TEXT("OnRep_EquipSlots called"));
    OnEquipSlotsUpdated.Broadcast();
}

bool UInventoryComponent::AddItem(FName ItemID)
{
    UE_LOG(LogTemp, Warning, TEXT("AddItem called"));

    if (!GetOwner()->HasAuthority())
    {
        UE_LOG(LogTemp, Warning, TEXT("AddItem: No authority"));
        return false;
    }

    for (int i = 0; i < InventorySlots.Num(); i++)
    {
        if (InventorySlots[i].bIsEmpty)
        {
            InventorySlots[i].ItemID = ItemID;
            InventorySlots[i].bIsEmpty = false;
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

    if (!EquipSlots[SlotIndex].bIsEmpty)
    {
        AddItem(EquipSlots[SlotIndex].ItemID);
    }

    EquipSlots[SlotIndex].ItemID = ItemID;
    EquipSlots[SlotIndex].bIsEmpty = false;
    InventorySlots[InventoryIndex].ItemID = NAME_None;
    InventorySlots[InventoryIndex].bIsEmpty = true;

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

    if (!AddItem(EquipSlots[EquipSlotIndex].ItemID))
    {
        return false;
    }

    EquipSlots[EquipSlotIndex].ItemID = NAME_None;
    EquipSlots[EquipSlotIndex].bIsEmpty = true;

    return true;
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UInventoryComponent, InventorySlots);
    DOREPLIFETIME(UInventoryComponent, EquipSlots);
}