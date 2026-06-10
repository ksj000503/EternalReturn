// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"


#include "CraftingComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCraftableListUpdated);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ETERNALRETURN_API UCraftingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCraftingComponent();

	// 제작 가능 목록
	UPROPERTY(ReplicatedUsing= OnRep_CraftableList, BlueprintReadOnly)
	TArray<FName> CraftableList;

	// 인벤토리 변경 시 호출
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void UpdateCraftableList();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void CrateItem();

	// UI 갱신 델리게이트
	UPROPERTY(BlueprintAssignable)
	FOnCraftableListUpdated OnCraftableListUpdated;

	// 재료 확인을 위한 데이터 테이블
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	TObjectPtr<UDataTable> ItemDataTable;

protected:
	UFUNCTION()
	void OnRep_CraftableList();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
