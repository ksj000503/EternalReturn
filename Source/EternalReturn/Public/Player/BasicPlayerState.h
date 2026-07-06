// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BasicPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerDied, ABasicPlayerState*, DeadPlayerState);

UCLASS()
class ETERNALRETURN_API ABasicPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	ABasicPlayerState();

	// 1. Replicated 대신 ReplicatedUsing을 써서 값이 복사 완료되면 OnRep_SelectName이 실행되게 합니다.
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_SelectName, Category = "Character Select")
	FName SelectName;

	// 생존 여부 (서버에서만 변경, 모든 클라이언트로 복제)
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_IsAlive, Category = "Combat")
	bool bIsAlive = true;

	// GameMode가 바인드해서 사망 신호를 받는 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Combat")
	FOnPlayerDied OnPlayerDied;

	// 서버 전용: 사망 처리 (ACombatEntityBase에서 호출)
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetDead();

	// 2. 값이 동기화되었을 때 클라이언트에서 실행될 함수입니다. (주로 UI 갱신 등을 넣습니다)
	UFUNCTION()
	void OnRep_SelectName();

	// 클라이언트: bIsAlive 복제 시 실행 (필요 시 UI 처리용)
	UFUNCTION()
	void OnRep_IsAlive();

	// 3. 지저분한 주석 기호를 지우고 깔끔하게 오버라이드 선언
	virtual void CopyProperties(APlayerState* PlayerState) override;

protected:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
