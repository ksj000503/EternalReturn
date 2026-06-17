// Fill out your copyright notice in the Description page of Project Settings.

#include "SkillComponent.h"
#include "TimerManager.h"

USkillComponent::USkillComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USkillComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

// ───────── 전술 스킬 (DataTable) ─────────

void USkillComponent::SetTacticalSkill(FName InRowName)
{
	if (!TacticalSkillTable)
	{
		return;
	}

	RowName = InRowName;

	const FTacticalSkillTableRow* FoundRow = TacticalSkillTable->FindRow<FTacticalSkillTableRow>(RowName, TEXT("SetTacticalSkill"));
	if (FoundRow)
	{
		TacticalSkill = *FoundRow;
	}
}

void USkillComponent::UseTacticalSkill(ETacticalSkillType InSkillType)
{
	// 전술 스킬은 F 슬롯 고정 (D는 무기 스킬 슬롯으로 별도 사용).
	const ESkillKeyType TacticalSlot = ESkillKeyType::Skill_F;

	if (!IsSkillReady(TacticalSlot))
	{
		return;
	}

	switch (InSkillType)
	{
	case ETacticalSkillType::Blink:
		Blink();
		break;
	case ETacticalSkillType::Quake:
		Quake();
		break;
	case ETacticalSkillType::ProtocolViolation:
		ProtocolViolation();
		break;
	case ETacticalSkillType::ElectricShift:
		ElectricShift();
		break;
	case ETacticalSkillType::ForceField:
		ForceField();
		break;
	case ETacticalSkillType::Totem:
		Totem();
		break;
	case ETacticalSkillType::Nullification:
		Nullification();
		break;
	case ETacticalSkillType::SoulStealer:
		SoulStealer();
		break;
	case ETacticalSkillType::TheStrijder:
		TheStrijder();
		break;
	case ETacticalSkillType::BladeofTruth:
		BladeofTruth();
		break;
	case ETacticalSkillType::HealingWind:
		HealingWind();
		break;
	default:
		break;
	}

	// 전술 스킬은 L1 고정.
	const float Cooldown = TacticalSkill.SkillStat.Cooldown_L1;
	StartCooldown(TacticalSlot, Cooldown);

	OnSkillExecuted.Broadcast(TacticalSlot);
}

// ───────── 캐릭터 Q/W/E/R 스킬 (DataAsset) ─────────

UDA_SkillBase* USkillComponent::GetSkillDataByKeyType(ESkillKeyType KeyType) const
{
	switch (KeyType)
	{
	case ESkillKeyType::Skill_Q:
		return Skill_Q_Data;
	case ESkillKeyType::Skill_W:
		return Skill_W_Data;
	case ESkillKeyType::Skill_E:
		return Skill_E_Data;
	case ESkillKeyType::Skill_R:
		return Skill_R_Data;
	default:
		return nullptr;
	}
}

void USkillComponent::ExecuteSkill(ESkillKeyType KeyType)
{
	if (!IsSkillReady(KeyType))
	{
		return;
	}

	UDA_SkillBase* SkillData = GetSkillDataByKeyType(KeyType);
	if (!SkillData)
	{
		return;
	}

	SkillType(KeyType, SkillData);

	StartCooldown(KeyType, SkillData->Cooldown);

	OnSkillExecuted.Broadcast(KeyType);
}

void USkillComponent::UseWeaponSkill()
{
	const ESkillKeyType WeaponSlot = ESkillKeyType::Skill_D;

	if (!IsSkillReady(WeaponSlot))
	{
		return;
	}

	switch (WeaponType)
	{
	case EWeaponType::Bow:
		Bow();
		break;
	case EWeaponType::TwoHandedSword:
		TwoHandedSword();
		break;
	default:
		break;
	}

	// 무기 스킬 쿨다운은 현재 DataAsset이 없으므로, 추후 무기별 쿨다운 값을 연결할 자리.
	// TODO: 무기 스킬 쿨다운 값 소스 확정 필요
}

void USkillComponent::SkillType(ESkillKeyType KeyType, UDA_SkillBase* SkillData)
{
	if (!SkillData)
	{
		return;
	}

	switch (SkillData->SkillType)
	{
	case ESkillType::Projectile:
		Projectile(KeyType, SkillData);
		break;
	case ESkillType::Dash:
		Dash(KeyType, SkillData);
		break;
	case ESkillType::AreaDamage:
		AreaDamage(KeyType, SkillData);
		break;
	case ESkillType::Buff:
		Buff(KeyType, SkillData);
		break;
	case ESkillType::CC:
		CC(KeyType, SkillData);
		break;
	case ESkillType::Toggle:
		Toggle(KeyType, SkillData);
		break;
	case ESkillType::Passive:
		Passive(KeyType, SkillData);
		break;
	default:
		break;
	}
}

// ───────── 스킬 타입별 실행 함수 (현재 빈 스텁, 추후 구현) ─────────

void USkillComponent::Projectile(ESkillKeyType KeyType, UDA_SkillBase* SkillData)
{
	UE_LOG(LogTemp, Warning, TEXT("Projectile"));
}

void USkillComponent::Dash(ESkillKeyType KeyType, UDA_SkillBase* SkillData)
{
	UE_LOG(LogTemp, Warning, TEXT("Dash"));
}

void USkillComponent::AreaDamage(ESkillKeyType KeyType, UDA_SkillBase* SkillData)
{
	UE_LOG(LogTemp, Warning, TEXT("AreaDamage"));
}

void USkillComponent::Buff(ESkillKeyType KeyType, UDA_SkillBase* SkillData)
{
	UE_LOG(LogTemp, Warning, TEXT("Buff"));
}

void USkillComponent::CC(ESkillKeyType KeyType, UDA_SkillBase* SkillData)
{
	UE_LOG(LogTemp, Warning, TEXT("CC"));
}

void USkillComponent::Toggle(ESkillKeyType KeyType, UDA_SkillBase* SkillData)
{
	bUseToggle = !bUseToggle;

	UE_LOG(LogTemp, Warning, TEXT("[SkillComponent] Toggle - bUseToggle = %s"), bUseToggle ? TEXT("true") : TEXT("false"));
}

void USkillComponent::Passive(ESkillKeyType KeyType, UDA_SkillBase* SkillData)
{
	UE_LOG(LogTemp, Warning, TEXT("Passive"));
}

// ───────── 전술 스킬 실행 함수 (현재 Blink만 작성, 나머지는 빈 스텁) ─────────

void USkillComponent::Blink()
{
	UE_LOG(LogTemp, Warning, TEXT("Blink"));
}

void USkillComponent::Quake()
{
	UE_LOG(LogTemp, Warning, TEXT("Quake"));
}

void USkillComponent::ProtocolViolation()
{
	UE_LOG(LogTemp, Warning, TEXT("ProtocolViolation"));
}

void USkillComponent::ElectricShift()
{
	UE_LOG(LogTemp, Warning, TEXT("ElectricShift"));
}

void USkillComponent::ForceField()
{
	UE_LOG(LogTemp, Warning, TEXT("ForceField"));
}

void USkillComponent::Totem()
{
	UE_LOG(LogTemp, Warning, TEXT("Totem"));
}

void USkillComponent::Nullification()
{
	UE_LOG(LogTemp, Warning, TEXT("Nullification"));
}

void USkillComponent::SoulStealer()
{
	UE_LOG(LogTemp, Warning, TEXT("SoulStealer"));
}

void USkillComponent::TheStrijder()
{
	UE_LOG(LogTemp, Warning, TEXT("TheStrijder"));
}

void USkillComponent::BladeofTruth()
{
	UE_LOG(LogTemp, Warning, TEXT("BladeofTruth"));
}

void USkillComponent::HealingWind()
{
	UE_LOG(LogTemp, Warning, TEXT("HealingWind"));
}

// ───────── 무기 스킬 실행 함수 (현재 빈 스텁, 추후 구현) ─────────

void USkillComponent::Bow()
{
}

void USkillComponent::TwoHandedSword()
{
}

// ───────── 쿨다운 처리 ─────────

bool USkillComponent::IsSkillReady(ESkillKeyType KeyType) const
{
	const bool* bReady = bCanUseSkill.Find(KeyType);

	// 처음 쓰는 슬롯은 Map에 항목이 없으므로 true(사용 가능)로 취급
	return bReady ? *bReady : true;
}

void USkillComponent::StartCooldown(ESkillKeyType KeyType, float Cooldown)
{
	if (Cooldown <= 0.f)
	{
		return;
	}

	bCanUseSkill.Add(KeyType, false);

	// KeyType을 변수에 따로 저장하지 않고, 타이머 콜백 자체에 바인딩해서 넘긴다.
	// 이렇게 하면 여러 스킬의 쿨다운이 동시에 진행되어도 서로 KeyType 값을 덮어쓰지 않는다.
	FTimerDelegate Delegate = FTimerDelegate::CreateUObject(this, &USkillComponent::HandleCooldownFinished, KeyType);

	FTimerHandle& Handle = CooldownTimerHandles.FindOrAdd(KeyType);
	GetWorld()->GetTimerManager().SetTimer(Handle, Delegate, Cooldown, false);
}

void USkillComponent::HandleCooldownFinished(ESkillKeyType KeyType)
{
	bCanUseSkill.Add(KeyType, true);

	OnCooldownFinished.Broadcast(KeyType);
}