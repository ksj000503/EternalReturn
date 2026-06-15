#include "CharacterStatComponent.h"
#include "Net/UnrealNetwork.h"

UCharacterStatComponent::UCharacterStatComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
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

    // 아이템 보너스 스탯 복제 등록 (추가)
    DOREPLIFETIME(UCharacterStatComponent, ItemMaxHP);
    DOREPLIFETIME(UCharacterStatComponent, ItemHPRegen);
    DOREPLIFETIME(UCharacterStatComponent, ItemAttackPower);
    DOREPLIFETIME(UCharacterStatComponent, ItemDefense);
    DOREPLIFETIME(UCharacterStatComponent, ItemMoveSpeed);
    DOREPLIFETIME(UCharacterStatComponent, ItemAttackSpeed);
    DOREPLIFETIME(UCharacterStatComponent, ItemAttackRange);
    DOREPLIFETIME(UCharacterStatComponent, ItemSkillAmplification);
    DOREPLIFETIME(UCharacterStatComponent, ItemCooldownReduction);
    DOREPLIFETIME(UCharacterStatComponent, ItemBasicAttackBonus);
    DOREPLIFETIME(UCharacterStatComponent, ItemDamageReduction);
    DOREPLIFETIME(UCharacterStatComponent, ItemLifeSteal);
    DOREPLIFETIME(UCharacterStatComponent, ItemCriticalChance);
    DOREPLIFETIME(UCharacterStatComponent, ItemCriticalDamage);
}

void UCharacterStatComponent::AddEXP(float Amount)
{
    CurrentEXP += Amount;

    if (CurrentEXP >= EXPToNextLevel)
    {
        LevelUp();
    }
}

void UCharacterStatComponent::LevelUp()
{
    if (Level >= 20) return;

    Level++;
    CurrentEXP = 0.f;
    RecalculateStats();
}

void UCharacterStatComponent::RecalculateStats()
{
    // TODO: 레벨 기반 Base 스탯 갱신 (추후 구현)
}

// 장착한 아이템 스탯을 Item 보너스 변수에 누적 (추가)
void UCharacterStatComponent::ApplyItemStats(const FS_ItemStatData& Stats)
{
    ItemMaxHP += Stats.MaxHp;
    ItemHPRegen += Stats.HPRegen;
    ItemAttackPower += Stats.AttackPower;
    ItemDefense += Stats.Defense;
    ItemMoveSpeed += Stats.MoveSpeed;
    ItemAttackSpeed += Stats.AttackSpeed;
    ItemAttackRange += Stats.AttackRange;
    ItemSkillAmplification += Stats.SkillAmplification;
    ItemCooldownReduction += Stats.CooldownReduction;
    ItemBasicAttackBonus += Stats.BasicAttackBonus;
    ItemDamageReduction += Stats.DamageReduction;
    ItemLifeSteal += Stats.LifeSteal;
    ItemCriticalChance += Stats.CriticalChance;
    ItemCriticalDamage += Stats.CriticalDamage;
}

// 해제한 아이템 스탯을 Item 보너스 변수에서 차감 (추가)
void UCharacterStatComponent::RemoveItemStats(const FS_ItemStatData& Stats)
{
    ItemMaxHP -= Stats.MaxHp;
    ItemHPRegen -= Stats.HPRegen;
    ItemAttackPower -= Stats.AttackPower;
    ItemDefense -= Stats.Defense;
    ItemMoveSpeed -= Stats.MoveSpeed;
    ItemAttackSpeed -= Stats.AttackSpeed;
    ItemAttackRange -= Stats.AttackRange;
    ItemSkillAmplification -= Stats.SkillAmplification;
    ItemCooldownReduction -= Stats.CooldownReduction;
    ItemBasicAttackBonus -= Stats.BasicAttackBonus;
    ItemDamageReduction -= Stats.DamageReduction;
    ItemLifeSteal -= Stats.LifeSteal;
    ItemCriticalChance -= Stats.CriticalChance;
    ItemCriticalDamage -= Stats.CriticalDamage;
}

void UCharacterStatComponent::OnRep_bIsEliminated()
{
    // 나중에 탈락 처리 로직 연동 시 여기서 처리
}