#include "InventoryComponent.h"
#include "EternalReturnCharacter.h"
#include "Net/UnrealNetwork.h"
#include "CraftingComponent.h"

UInventoryComponent::UInventoryComponent()
{
    // Tick 비활성화 (필요 없음)
    PrimaryComponentTick.bCanEverTick = false;
    // 컴포넌트 복제 활성화
    SetIsReplicatedByDefault(true);
}

void UInventoryComponent::BeginPlay()
{
    Super::BeginPlay();

    // 서버에서만 슬롯 초기화 (클라이언트는 복제로 받음)
    if (GetOwner()->HasAuthority())
    {
        // 인벤토리 슬롯 10개 초기화
        for (int i = 0; i < 10; i++)
        {
            FInventorySlot Slot;
            Slot.bIsEmpty = true;
            InventorySlots.Add(Slot);
        }

        // 장비 슬롯 5개 초기화
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
    // 클라이언트에서 인벤토리 복제 완료 시 UI 갱신 델리게이트 호출
    OnInventoryUpdated.Broadcast();
}

void UInventoryComponent::OnRep_EquipSlots()
{
    UE_LOG(LogTemp, Warning, TEXT("OnRep_EquipSlots called"));
    // 클라이언트에서 장비 슬롯 복제 완료 시 UI 갱신 델리게이트 호출
    OnEquipSlotsUpdated.Broadcast();
}

// CraftingComponent를 가져오는 헬퍼 함수 (반복 코드 제거)
UCraftingComponent* UInventoryComponent::GetCraftingComponent()
{
    return Cast<UCraftingComponent>(GetOwner()->GetComponentByClass(UCraftingComponent::StaticClass()));
}

bool UInventoryComponent::AddItem(FName ItemID)
{
    UE_LOG(LogTemp, Warning, TEXT("AddItem called"));

    // 서버에서만 실행
    if (!GetOwner()->HasAuthority())
    {
        UE_LOG(LogTemp, Warning, TEXT("AddItem: No authority"));
        return false;
    }

    // 빈 슬롯 찾아서 아이템 추가
    for (int i = 0; i < InventorySlots.Num(); i++)
    {
        if (InventorySlots[i].bIsEmpty)
        {
            InventorySlots[i].ItemID = ItemID;
            InventorySlots[i].bIsEmpty = false;

            // 인벤토리 변경 후 제작 가능 목록 갱신
            UCraftingComponent* Crafting = GetCraftingComponent();
            if (Crafting)
            {
                Crafting->UpdateCraftableList();
            }

            return true;
        }
    }

    // 빈 슬롯 없음
    return false;
}

bool UInventoryComponent::RemoveItem(FName ItemID)
{
    // 서버에서만 실행
    if (!GetOwner()->HasAuthority())
    {
        return false;
    }

    // 해당 아이템 찾아서 제거
    for (int i = 0; i < InventorySlots.Num(); i++)
    {
        if (InventorySlots[i].ItemID == ItemID)
        {
            InventorySlots[i].bIsEmpty = true;
            InventorySlots[i].ItemID = NAME_None;

            // 인벤토리 변경 후 제작 가능 목록 갱신
            UCraftingComponent* Crafting = GetCraftingComponent();
            if (Crafting)
            {
                Crafting->UpdateCraftableList();
            }

            return true;
        }
    }

    // 아이템 없음
    return false;
}

bool UInventoryComponent::EquipItem(int32 InventoryIndex)
{
    // 서버에서만 실행
    if (!GetOwner()->HasAuthority())
    {
        return false;
    }

    // 유효한 인덱스인지 확인
    if (!InventorySlots.IsValidIndex(InventoryIndex))
    {
        return false;
    }

    // 슬롯이 비어있으면 장착 불가
    if (InventorySlots[InventoryIndex].bIsEmpty)
    {
        return false;
    }

    FName ItemID = InventorySlots[InventoryIndex].ItemID;

    // DataTable에서 아이템 데이터 조회
    FS_ItemData* ItemData = ItemDataTable->FindRow<FS_ItemData>(ItemID, TEXT(""));
    if (ItemData == nullptr)
    {
        return false;
    }

    int32 SlotIndex = -1;

    // 아이템 타입에 따라 장비 슬롯 인덱스 결정
    switch (ItemData->ItemType)
    {
    case EItemType::Weapon:
    {
        // 캐릭터의 허용 무기 타입과 일치하는지 확인
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

    // 지원하지 않는 아이템 타입
    if (SlotIndex == -1)
    {
        return false;
    }

    // 장비 슬롯에 이미 아이템이 있으면 인벤토리로 돌려보냄
    if (!EquipSlots[SlotIndex].bIsEmpty)
    {
        AddItem(EquipSlots[SlotIndex].ItemID);
    }

    // 장비 슬롯에 아이템 장착
    EquipSlots[SlotIndex].ItemID = ItemID;
    EquipSlots[SlotIndex].bIsEmpty = false;

    // 인벤토리 슬롯 비우기
    InventorySlots[InventoryIndex].ItemID = NAME_None;
    InventorySlots[InventoryIndex].bIsEmpty = true;

    return true;
}

bool UInventoryComponent::UnequipItem(int32 EquipSlotIndex)
{
    // 서버에서만 실행
    if (!GetOwner()->HasAuthority())
    {
        return false;
    }

    // 유효한 인덱스인지 확인
    if (!EquipSlots.IsValidIndex(EquipSlotIndex))
    {
        return false;
    }

    // 슬롯이 비어있으면 해제 불가
    if (EquipSlots[EquipSlotIndex].bIsEmpty)
    {
        return false;
    }

    // 인벤토리에 공간 없으면 해제 불가
    if (!AddItem(EquipSlots[EquipSlotIndex].ItemID))
    {
        return false;
    }

    // 장비 슬롯 비우기
    EquipSlots[EquipSlotIndex].ItemID = NAME_None;
    EquipSlots[EquipSlotIndex].bIsEmpty = true;

    return true;
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // 인벤토리 슬롯과 장비 슬롯 복제 등록
    DOREPLIFETIME(UInventoryComponent, InventorySlots);
    DOREPLIFETIME(UInventoryComponent, EquipSlots);
}