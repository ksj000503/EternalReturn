// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStatComponent.h"

// Sets default values for this component's properties
UCharacterStatComponent::UCharacterStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


void UCharacterStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UCharacterStatComponent, Level);

    DOREPLIFETIME(UCharacterStatComponent, CurrentEXP);
    
    DOREPLIFETIME(UCharacterStatComponent, EXPToNextLevel);
    
    DOREPLIFETIME(UCharacterStatComponent, SkillAmplification);
    
    DOREPLIFETIME(UCharacterStatComponent, CooldownReduction);
    
    DOREPLIFETIME(UCharacterStatComponent, BasicAttackBonus);
    
    DOREPLIFETIME(UCharacterStatComponent, DamageReduction);
    
    DOREPLIFETIME(UCharacterStatComponent, LifeSteal);
    
    DOREPLIFETIME(UCharacterStatComponent, CriticalChance);
    
    DOREPLIFETIME(UCharacterStatComponent, CriticalDamage);
    
    DOREPLIFETIME(UCharacterStatComponent, VisionRange);
    
    DOREPLIFETIME(UCharacterStatComponent, MaxGauge);
    
    DOREPLIFETIME(UCharacterStatComponent, CurrentGauge);
    
    DOREPLIFETIME(UCharacterStatComponent, bIsEliminated);
}

void UCharacterStatComponent::AddEXP(float Amount)
{
    // 경험치 추가후 조건 충족 시 레벨업

    CurrentEXP += Amount;

    if (CurrentEXP >= EXPToNextLevel)
    {
        LevelUp();
    }
}

void UCharacterStatComponent::LevelUp()
{
    // 레벨 업 후 경험치 초기화 + 추가 스탯 처리

    if (Level >= 20) return;

    Level++;

    CurrentEXP = 0.f;

    RecalculateStats();
}

void UCharacterStatComponent::RecalculateStats()
{
    // TODO: DataTable에서 레벨 기반 Base 스탯 읽어오기
    // TODO: InventoryComponent에서 아이템 보너스 합산
}

void UCharacterStatComponent::OnRep_bIsEliminated()
{
    // 나중에 탈락 처리 로직 연동 시 여기서 처리
}