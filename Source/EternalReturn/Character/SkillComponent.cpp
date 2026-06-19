// Fill out your copyright notice in the Description page of Project Settings.

#include "SkillComponent.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include <Navigation/PathFollowingComponent.h>
#include <GameFramework/ProjectileMovementComponent.h>

USkillComponent::USkillComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void USkillComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsDashing)
	{
		AActor* OwnerActor = GetOwner();
		if (!OwnerActor)
		{
			bIsDashing = false;
			return;
		}

		const float StepDistance = FMath::Min(DashSpeed * DeltaTime, DashRemainingDistance);
		const FVector StepOffset = DashDirection * StepDistance;

		FHitResult Hit;
		OwnerActor->SetActorLocation(OwnerActor->GetActorLocation() + StepOffset, true, &Hit);

		DashRemainingDistance -= StepDistance;

		// 목표 거리를 다 이동했거나, 벽 같은 곳에 막히면 대시 종료.
		if (DashRemainingDistance <= 0.f || Hit.bBlockingHit)
		{
			bIsDashing = false;
		}
	}
}

void USkillComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USkillComponent, bUseToggle);
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

void USkillComponent::UseTacticalSkill(ETacticalSkillType InSkillType, FVector TargetLocation)
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
		Blink(TargetLocation);
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

void USkillComponent::ExecuteSkill(ESkillKeyType KeyType, FVector TargetLocation)
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

	SkillType(KeyType, SkillData, TargetLocation);

	StartCooldown(KeyType, SkillData->Cooldown);

	OnSkillExecuted.Broadcast(KeyType);
}

void USkillComponent::UseWeaponSkill(FVector TargetLocation)
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
	// TODO: Bow/TwoHandedSword도 TargetLocation 필요해지면 여기서 같이 넘겨주면 됨.
}

void USkillComponent::SkillType(ESkillKeyType KeyType, UDA_SkillBase* SkillData, const FVector& TargetLocation)
{
	if (!SkillData)
	{
		return;
	}

	switch (SkillData->SkillType)
	{
	case ESkillType::Projectile:
		Projectile(KeyType, SkillData, TargetLocation);
		break;
	case ESkillType::Dash:
		Dash(KeyType, SkillData, TargetLocation);
		break;
	case ESkillType::AreaDamage:
		AreaDamage(KeyType, SkillData, TargetLocation);
		break;
	case ESkillType::Buff:
		Buff(KeyType, SkillData, TargetLocation);
		break;
	case ESkillType::CC:
		CC(KeyType, SkillData, TargetLocation);
		break;
	case ESkillType::Toggle:
		Toggle(KeyType, SkillData, TargetLocation);
		break;
	case ESkillType::Passive:
		Passive(KeyType, SkillData, TargetLocation);
		break;
	default:
		break;
	}
}

// ───────── 스킬 타입별 실행 함수 (현재 Dash만 작성, 나머지는 빈 스텁) ─────────

void USkillComponent::Projectile(ESkillKeyType KeyType, UDA_SkillBase* SkillData, const FVector& TargetLocation)
{
	//AActor* OwnerActor = GetOwner();
	//if (!OwnerActor || !SkillData || !SkillData->ProjectileClass) return;

	//// 1. 발사 위치 및 방향 계산
	//FVector SpawnLocation = OwnerActor->GetActorLocation() + (OwnerActor->GetActorForwardVector() * 50.0f);
	//FVector Direction = (TargetLocation - OwnerActor->GetActorLocation()).GetSafeNormal();
	//FRotator SpawnRotation = Direction.Rotation();

	//FActorSpawnParameters SpawnParams;
	//SpawnParams.Owner = OwnerActor;
	//SpawnParams.Instigator = Cast<APawn>(OwnerActor);

	//// 2. 화살 스폰
	//AActor* SpawnedProjectile = GetWorld()->SpawnActor<AActor>(SkillData->ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);

	//// 3. 화살 설정 (Initialize 함수 호출)
	//if (SpawnedProjectile)
	//{
	//	// 화살 클래스(AMyNewArrow)로 형변환
	//	// ※ 주의: AMyNewArrow.h를 반드시 #include 해야 합니다.
	//	AMyNewArrow* NewArrow = Cast<AMyNewArrow>(SpawnedProjectile);

	//	if (NewArrow)
	//	{
	//		// 데이터 에셋에서 받아온 값으로 화살 초기화
	//		NewArrow->Initialize(SkillData->Range, SkillData->Speed);
	//	}
	//	else
	//	{
	//		// 만약 화살 클래스가 Initialize를 지원하지 않는다면 기본 컴포넌트 강제 설정
	//		UProjectileMovementComponent* MovementComp = SpawnedProjectile->FindComponentByClass<UProjectileMovementComponent>();
	//		if (MovementComp)
	//		{
	//			MovementComp->InitialSpeed = SkillData->Speed;
	//			MovementComp->MaxSpeed = SkillData->Speed;
	//			SpawnedProjectile->SetLifeSpan(SkillData->Range / SkillData->Speed);
	//		}
	//	}
	//}
	
	UE_LOG(LogTemp, Warning, TEXT("Projectile"));
}

void USkillComponent::Dash(ESkillKeyType KeyType, UDA_SkillBase* SkillData, const FVector& TargetLocation)
{
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor || !OwnerActor->HasAuthority())
	{
		return;
	}

	FVector Direction = TargetLocation - OwnerActor->GetActorLocation();
	Direction.Z = 0.f;
	Direction = Direction.GetSafeNormal();

	if (Direction.IsNearlyZero())
	{
		return;
	}

	// SkillData->Range를 "대시로 이동할 거리"로 사용. DashDuration 동안 그 거리를 이동하도록 속도를 역산한다.
	// (Blink의 Range가 "최대 이동 거리"였던 것과 같은 의미로 통일)
	const float DashDistance = SkillData ? SkillData->Range : 0.f;
	const float DashDuration = 0.2f; // 임시값. 캐릭터마다 다르게 하고 싶으면 DataAsset에 필드 추가해서 빼면 됨.

	if (DashDistance <= 0.f)
	{
		return;
	}

	DashDirection = Direction;
	DashRemainingDistance = DashDistance;
	DashSpeed = DashDistance / DashDuration;
	bIsDashing = true;

	UE_LOG(LogTemp, Warning, TEXT("[SkillComponent] Dash start -> Direction=%s Distance=%.1f Speed=%.1f"), *Direction.ToString(), DashDistance, DashSpeed);
}

void USkillComponent::AreaDamage(ESkillKeyType KeyType, UDA_SkillBase* SkillData, const FVector& TargetLocation)
{
	UE_LOG(LogTemp, Warning, TEXT("AreaDamage"));
}

void USkillComponent::Buff(ESkillKeyType KeyType, UDA_SkillBase* SkillData, const FVector& TargetLocation)
{
	UE_LOG(LogTemp, Warning, TEXT("Buff"));
}

void USkillComponent::CC(ESkillKeyType KeyType, UDA_SkillBase* SkillData, const FVector& TargetLocation)
{
	UE_LOG(LogTemp, Warning, TEXT("CC"));
}

void USkillComponent::Toggle(ESkillKeyType KeyType, UDA_SkillBase* SkillData, const FVector& TargetLocation)
{
	bUseToggle = !bUseToggle;

	UE_LOG(LogTemp, Warning, TEXT("[SkillComponent] Toggle - bUseToggle = %s"), bUseToggle ? TEXT("true") : TEXT("false"));
}

void USkillComponent::Passive(ESkillKeyType KeyType, UDA_SkillBase* SkillData, const FVector& TargetLocation)
{
	UE_LOG(LogTemp, Warning, TEXT("Passive"));
}

// ───────── 전술 스킬 실행 함수 (현재 Blink만 작성, 나머지는 빈 스텁) ─────────

void USkillComponent::Blink(const FVector& TargetLocation)
{
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor || !OwnerActor->HasAuthority())
	{
		return;
	}

	// SetTacticalSkill로 DataTable에서 Range를 아직 안 채워놨으면 0이라서,
	// 일단 테스트 단계에서는 0이면 거리 제한 없이 마우스 위치로 그대로 이동하게 둔다.
	const float MaxRange = TacticalSkill.SkillStat.Range;

	const FVector CurrentLocation = OwnerActor->GetActorLocation();
	const FVector Delta = TargetLocation - CurrentLocation;
	const float Distance = Delta.Size();

	FVector FinalLocation;
	if (MaxRange <= 0.f || Distance <= MaxRange)
	{
		// 마우스가 사거리 안이면 그 지점으로 정확히 이동.
		FinalLocation = TargetLocation;
	}
	else
	{
		// 사거리 밖이면 마우스 방향으로 MaxRange만큼만 이동.
		FinalLocation = CurrentLocation + Delta.GetSafeNormal() * MaxRange;
	}

	OwnerActor->SetActorLocation(FinalLocation, false, nullptr, ETeleportType::TeleportPhysics);

	UE_LOG(LogTemp, Warning, TEXT("[SkillComponent] Blink -> %s (Distance=%.1f, MaxRange=%.1f)"), *FinalLocation.ToString(), Distance, MaxRange);
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
	UE_LOG(LogTemp, Warning, TEXT("Bow"));
}

void USkillComponent::TwoHandedSword()
{
	UE_LOG(LogTemp, Warning, TEXT("TwoHandedSword"));
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

	// 서버 권위 판정용 종료 타이머 (KeyType을 콜백에 바로 바인딩해서 슬롯 간 충돌 없음).
	FTimerDelegate Delegate = FTimerDelegate::CreateUObject(this, &USkillComponent::HandleCooldownFinished, KeyType);

	FTimerHandle& Handle = CooldownTimerHandles.FindOrAdd(KeyType);
	GetWorld()->GetTimerManager().SetTimer(Handle, Delegate, Cooldown, false);

	// 이 액터를 소유한 클라이언트에게 "표시용" 쿨다운 정보를 전달.
	// 서버가 직접 호출하므로 호스트(서버=클라1)에서도 동일하게 한 번 거치게 되어 일관됨.
	Client_StartCooldownDisplay(KeyType, Cooldown);
}

void USkillComponent::Client_StartCooldownDisplay_Implementation(ESkillKeyType KeyType, float Cooldown)
{
	// 진행률 계산용 정보 저장 (시작 시각, 총 쿨다운) — 클라이언트 로컬에서만 사용.
	FCooldownInfo Info;
	Info.TotalCooldown = Cooldown;
	Info.StartTime = GetWorld()->GetTimeSeconds();
	CooldownInfoMap.Add(KeyType, Info);

	// 0.1초 간격으로 진행률만 갱신하는 별도 타이머 (Tick 미사용, UI 표시용). 클라이언트 로컬 타이머.
	FTimerDelegate UpdateDelegate = FTimerDelegate::CreateUObject(this, &USkillComponent::UpdateCooldownPercent, KeyType);
	FTimerHandle& UpdateHandle = CooldownUpdateTimerHandles.FindOrAdd(KeyType);
	GetWorld()->GetTimerManager().SetTimer(UpdateHandle, UpdateDelegate, 0.1f, true);

	// UI가 "쿨다운 시작됨" 시점을 알 수 있도록 클라이언트 로컬로도 Broadcast.
	// (서버 쪽 OnSkillExecuted는 실제 스킬 실행 로직용이고, 이건 순수 표시 트리거용)
	OnSkillExecuted.Broadcast(KeyType);

	// 시작 즉시 100%(1.0), 남은시간=Cooldown으로 한 번 알려준다.
	OnCooldownUpdated.Broadcast(KeyType, 1.0f, Cooldown);
}

void USkillComponent::HandleCooldownFinished(ESkillKeyType KeyType)
{
	// 서버 권위 판정: 이제 다시 사용 가능.
	bCanUseSkill.Add(KeyType, true);

	OnCooldownFinished.Broadcast(KeyType);
}

void USkillComponent::UpdateCooldownPercent(ESkillKeyType KeyType)
{
	const FCooldownInfo* Info = CooldownInfoMap.Find(KeyType);
	if (!Info || Info->TotalCooldown <= 0.f)
	{
		return;
	}

	const float Elapsed = GetWorld()->GetTimeSeconds() - Info->StartTime;
	const float Remaining = FMath::Clamp(Info->TotalCooldown - Elapsed, 0.f, Info->TotalCooldown);

	// 1.0(막 사용) → 0.0(사용 가능)으로 떨어지는 비율
	const float Percent = Remaining / Info->TotalCooldown;

	OnCooldownUpdated.Broadcast(KeyType, Percent, Remaining);

	// 표시용 진행률이 0에 도달하면, 이 타이머는 더 이상 필요 없으니 스스로 정리.
	// (서버 권위 판정인 bCanUseSkill은 HandleCooldownFinished가 별도로 갱신함)
	if (Percent <= 0.f)
	{
		if (FTimerHandle* UpdateHandle = CooldownUpdateTimerHandles.Find(KeyType))
		{
			GetWorld()->GetTimerManager().ClearTimer(*UpdateHandle);
		}
		CooldownInfoMap.Remove(KeyType);

		// UI가 "쿨다운 끝남"(숫자/오버레이 숨김) 시점을 알 수 있도록 클라이언트 로컬로도 Broadcast.
		OnCooldownFinished.Broadcast(KeyType);
	}
}