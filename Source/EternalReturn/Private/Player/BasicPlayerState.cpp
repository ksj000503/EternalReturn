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
		// [로그 확인용] 에러(빨간색) 로그로 출력 창에 진짜 이 함수가 실행되는지 체크!
		UE_LOG(LogTemp, Error, TEXT("!!복사 함수 작동 시작!! 기존 값: %s"), *SelectName.ToString());

		// 클로드 말대로 this 빼고 깔끔하게 대입
		NewPS->SelectName = SelectName;

		UE_LOG(LogTemp, Error, TEXT("!!복사 완료!! 새 PlayerState에 들어간 값: %s"), *NewPS->SelectName.ToString());
	}
}

void ABasicPlayerState::OnRep_SelectName()
{
	// 클라이언트나 리슨서버 화면에 값이 도착하면 이 로그가 찍힙니다.
	UE_LOG(LogTemp, Warning, TEXT("클라이언트에 SelectName 동기화 완료: %s"), *SelectName.ToString());

	// UI 업데이트나 캐릭터 변경 로직이 필요하다면 여기서 호출하세요!
}

// 서버 전용: 사망 처리
// ACombatEntityBase::OnDeath_Implementation()에서 호출
void ABasicPlayerState::SetDead()
{
	if (!HasAuthority()) return;
	if (!bIsAlive) return;

	bIsAlive = false;

	UE_LOG(LogTemp, Warning, TEXT("[SetDead] 호출됨, PlayerState=%s"), *GetName());

	OnRep_IsAlive();
}


// 클라이언트: bIsAlive 복제 시 실행
void ABasicPlayerState::OnRep_IsAlive()
{
	UE_LOG(LogTemp, Warning, TEXT("[OnRep_IsAlive] Broadcast 시도, bIsAlive=%d"), bIsAlive);

	// GameMode가 바인드한 델리게이트 브로드캐스트
	// (서버에서는 SetDead()가 직접 호출, 클라이언트에서는 리플리케이션으로 자동 호출됨)
	OnPlayerDied.Broadcast(this);
}


void ABasicPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABasicPlayerState, SelectName);
	DOREPLIFETIME(ABasicPlayerState, bIsAlive);
}