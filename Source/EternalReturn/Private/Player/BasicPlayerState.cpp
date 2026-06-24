// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/BasicPlayerState.h"
#include "GameFramework/GameModeBase.h"
#include "Net/UnrealNetwork.h"

ABasicPlayerState::ABasicPlayerState()
{
	SelectName = NAME_None;
}

void ABasicPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	// 인자로 들어온 이전 레벨의 가방(OldPS)을 캐스팅합니다.
	if (ABasicPlayerState* OldPS = Cast<ABasicPlayerState>(PlayerState))
	{
		// 1. ⭕ 올바른 방향: 이전 레벨 가방(OldPS)의 데이터를 새로 태어난 내 가방(this)에 복사합니다.
		this->SelectName = OldPS->SelectName;

		// 2. ⭐️ 데이터 복사가 "완벽히 끝난 직후" 게임모드에게 스폰을 요청합니다.
		if (UWorld* World = this->GetWorld())
		{
			if (AGameModeBase* GM = World->GetAuthGameMode())
			{
				// 현재 새롭게 연결된 플레이어 컨트롤러를 가져와 게임모드의 RestartPlayer를 때립니다.
				if (AController* PC = this->GetPlayerController())
				{
					GM->RestartPlayer(PC);
				}
			}
		}
	}
}

void ABasicPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// ⭐️ 여기를 ABasicPlayerState 로 변경했습니다!
	DOREPLIFETIME(ABasicPlayerState, SelectName);
}