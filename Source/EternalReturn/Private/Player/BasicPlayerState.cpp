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

	if (ABasicPlayerState* NewPS = Cast<ABasicPlayerState>(PlayerState))
	{
		NewPS->SelectName = SelectName;
	}
}

void ABasicPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABasicPlayerState, SelectName);
}