// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BasicPlayerState.generated.h"

UCLASS()
class ETERNALRETURN_API ABasicPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ABasicPlayerState();

	// 1. Replicated 대신 ReplicatedUsing을 써서 값이 복사 완료되면 OnRep_SelectName이 실행되게 합니다.
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_SelectName, Category = "Character Select")
	FName SelectName;

	// 2. 값이 동기화되었을 때 클라이언트에서 실행될 함수입니다. (주로 UI 갱신 등을 넣습니다)
	UFUNCTION()
	void OnRep_SelectName();

	// 3. 지저분한 주석 기호를 지우고 깔끔하게 오버라이드 선언
	virtual void CopyProperties(APlayerState* PlayerState) override;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};