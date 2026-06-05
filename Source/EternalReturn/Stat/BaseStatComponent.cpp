// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseStatComponent.h"

// Sets default values for this component's properties
UBaseStatComponent::UBaseStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
	// ...
}

void UBaseStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UBaseStatComponent, CurrentHP);

    DOREPLIFETIME(UBaseStatComponent, MaxHP);

    DOREPLIFETIME(UBaseStatComponent, HPRegen);

    DOREPLIFETIME(UBaseStatComponent, AttackPower);

    DOREPLIFETIME(UBaseStatComponent, Defense);

    DOREPLIFETIME(UBaseStatComponent, MoveSpeed);

    DOREPLIFETIME(UBaseStatComponent, AttackSpeed);

    DOREPLIFETIME(UBaseStatComponent, AttackRange);

    DOREPLIFETIME(UBaseStatComponent, bIsDead);
}


void UBaseStatComponent::SetMaxHP(float Value)
{
   // 0 미만으로 내려가지 않게 Clamp
    MaxHP = FMath::Max(0.f, Value);
}

void UBaseStatComponent::SetCurrentHP(float Value)
{
    // 0~MaxHP 사이로 Clamp
    CurrentHP = FMath::Clamp(Value, 0.f, MaxHP);

    OnRep_CurrentHP();
}

void UBaseStatComponent::SetHPRegen(float Value) 
{ 
    HPRegen = Value; 
}

void UBaseStatComponent::SetAttackPower(float Value) 
{ 
    AttackPower = Value; 
}

void UBaseStatComponent::SetDefense(float Value) 
{ 
    Defense = Value; 
}

void UBaseStatComponent::SetMoveSpeed(float Value) 
{ 
    MoveSpeed = Value; 
}

void UBaseStatComponent::SetAttackSpeed(float Value) 
{ 
    AttackSpeed = Value; 
}

void UBaseStatComponent::SetAttackRange(float Value)
{
    AttackRange = FMath::Max(0.f, Value);

    OnRep_AttackRange();
}

void UBaseStatComponent::ApplyDamage(float DamageAmount)
{
    if (!IsAlive()) return;

    // 방어력 차감 후 데미지 처리
    const float ActualDamage = FMath::Max(0.f, DamageAmount - Defense);
    SetCurrentHP(CurrentHP - ActualDamage);

    if (CurrentHP <= 0.f && !bIsDead)
    {
        bIsDead = true;
        OnRep_bIsDead();
    }
}

void UBaseStatComponent::Heal(float HealAmount)
{
    if (!IsAlive()) return;
    SetCurrentHP(CurrentHP + HealAmount);
}

bool UBaseStatComponent::IsAlive() const
{
    return !bIsDead && CurrentHP > 0.f;
}

void UBaseStatComponent::OnRep_CurrentHP()
{
    OnHPChanged.Broadcast(CurrentHP, MaxHP);
}

void UBaseStatComponent::OnRep_bIsDead()
{
    if (bIsDead)
    {
        OnDeath.Broadcast();
    }
}

void UBaseStatComponent::OnRep_AttackRange()
{
    // 나중에 SphereCollision 반경 변경 연동 시 여기서 처리
}