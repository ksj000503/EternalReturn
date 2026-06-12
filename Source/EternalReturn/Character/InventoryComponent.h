#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ERTypes.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEquipSlotsUpdated);

class UCraftingComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ETERNALRETURN_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UInventoryComponent();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    UDataTable* ItemDataTable;

    UFUNCTION()
    void OnRep_InventorySlots();

    UFUNCTION()
    void OnRep_EquipSlots();

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UPROPERTY(ReplicatedUsing = OnRep_InventorySlots, BlueprintReadOnly, Category = "Inventory")
    TArray<FInventorySlot> InventorySlots;

    UPROPERTY(ReplicatedUsing = OnRep_EquipSlots, BlueprintReadOnly, Category = "Inventory")
    TArray<FInventorySlot> EquipSlots;


    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool AddItem(FName ItemID);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool RemoveItem(FName ItemID);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool EquipItem(int32 InventoryIndex);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool UnequipItem(int32 EquipSlotIndex);

    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnInventoryUpdated OnInventoryUpdated;

    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnEquipSlotsUpdated OnEquipSlotsUpdated;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    // CraftingComponent ÇïÆÛ ÇÔ¼ö
    UCraftingComponent* GetCraftingComponent();
};