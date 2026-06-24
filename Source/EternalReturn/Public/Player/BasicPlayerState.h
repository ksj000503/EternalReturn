// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BasicPlayerState.generated.h"

/**
 * Seamless Travel 시 새로 생성되는 PlayerState로 캐릭터 선택 정보를 복사하기 위한 C++ PlayerState.
 *
 * CopyProperties()는 BlueprintImplementableEvent/BlueprintNativeEvent가 아닌
 * 순수 C++ virtual 함수라서 Blueprint에서는 Override가 불가능합니다.
 * 따라서 BP_PlayerState의 부모 클래스를 반드시 이 클래스로 변경(Reparent)해야 합니다.
 *
 * 주의: ETERNALRETURN_API는 추측한 매크로입니다.
 * 프로젝트의 기존 C++ 클래스(예: USkillComponent.h, AStructureBase.h 등) 상단을 열어
 * 실제 사용 중인 모듈 API 매크로 이름을 확인하고 그대로 맞춰서 교체해주세요.
 */
UCLASS()
class ETERNALRETURN_API ABasicPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ABasicPlayerState();

	/** Lobby에서 선택한 캐릭터의 DataTable Row Name (예: "Rio", "Yuki") */
	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Character Select")
	FName SelectName;

	// 만약 C2S_SetCharacterName의 "Tactical Type"(예: 블링크) 같은 값도
	// InGame까지 같이 넘어가야 한다면, 아래처럼 변수를 추가하고
	// CopyProperties()에도 같은 패턴으로 복사 로직을 추가해주세요.
	//
	// UPROPERTY(BlueprintReadWrite, Replicated, Category = "Character Select")
	// FName SelectTacticalType;

	//~ Begin APlayerState Interface
	/** Seamless Travel 시 호출되어, 이전 PlayerState의 값을 새 PlayerState로 복사합니다. */
	virtual void CopyProperties(APlayerState* PlayerState) override;
	//~ End APlayerState Interface

protected:
	//~ Begin AActor Interface
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End AActor Interface
};