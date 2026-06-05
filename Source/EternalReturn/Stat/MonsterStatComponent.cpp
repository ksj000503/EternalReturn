#include "Stat/MonsterStatComponent.h"

UMonsterStatComponent::UMonsterStatComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UMonsterStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // AI 전용 변수는 복제 불필요 — 서버만 사용
}

void UMonsterStatComponent::InitFromDataTable(FName RowName)
{
    // TODO: DT_MonsterData에서 RowName으로 읽어서 Setter 호출
    // BP_MonsterBase의 InitializeStats 대체 예정
}