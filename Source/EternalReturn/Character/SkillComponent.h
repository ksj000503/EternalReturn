// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "ERTypes.h"
#include "DA_SkillBase.h"
#include "SkillComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillExecuted, ESkillKeyType, KeyType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCooldownFinished, ESkillKeyType, KeyType);

UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class ETERNALRETURN_API USkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USkillComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ───────── 전술 스킬 (DataTable) ─────────

	// 에디터에서 직접 DT_TacticalSkillData를 꽂아두는 DataTable 참조.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Tactical")
	UDataTable* TacticalSkillTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Tactical")
	FName RowName;

	// 로비에서 선택한 전술 스킬의 RowName으로 DataTable을 조회해 채워지는 구조체.
	UPROPERTY(BlueprintReadOnly, Category = "Skill|Tactical")
	FTacticalSkillTableRow TacticalSkill;

	// RowName으로 TacticalSkillTable을 조회해서 TacticalSkill을 채운다.
	UFUNCTION(BlueprintCallable, Category = "Skill|Tactical")
	void SetTacticalSkill(FName InRowName);

	// 전술 스킬 사용. TacticalSkill.SkillType(블링크/지진/...)으로 분기해서 실행한다.
	UFUNCTION(BlueprintCallable, Category = "Skill|Tactical")
	void UseTacticalSkill(ETacticalSkillType InSkillType);

	// ───────── 캐릭터 Q/W/E/R 스킬 (DataAsset) ─────────

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Character")
	UDA_SkillBase* Skill_Q_Data;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Character")
	UDA_SkillBase* Skill_W_Data;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Character")
	UDA_SkillBase* Skill_E_Data;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Character")
	UDA_SkillBase* Skill_R_Data;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Character")
	bool bUseToggle = false;

	// 무기 스킬(Skill_D)에서 어떤 무기 타입으로 분기할지 — DataTable 대신 에디터에서 직접 선택.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Weapon")
	EWeaponType WeaponType = EWeaponType::None;

	// 캐릭터 BP의 C2S_SkillQ/W/E/R/D/F 에서 그대로 호출하는 진입점.
	// 쿨다운 체크 → KeyType에 맞는 DataAsset을 골라 SkillType으로 라우팅.
	UFUNCTION(BlueprintCallable, Category = "Skill|Character")
	void ExecuteSkill(ESkillKeyType KeyType);

	// 무기 스킬(Skill_D) 진입점. WeaponType으로 분기해서 실행한다.
	UFUNCTION(BlueprintCallable, Category = "Skill|Weapon")
	void UseWeaponSkill();

protected:
	// KeyType에 해당하는 Skill_*_Data를 반환 (Skill_D, Skill_F는 현재 캐릭터 스킬 DataAsset 미사용 → nullptr)
	UDA_SkillBase* GetSkillDataByKeyType(ESkillKeyType KeyType) const;

	// DataAsset의 SkillType Enum을 보고 실제 스킬 함수로 분기
	void SkillType(ESkillKeyType KeyType, UDA_SkillBase* SkillData);

	// ───────── 스킬 타입별 실행 함수 (DataAsset 기반, 캐릭터 Q/W/E/R) ─────────
	void Projectile(ESkillKeyType KeyType, UDA_SkillBase* SkillData);
	void Dash(ESkillKeyType KeyType, UDA_SkillBase* SkillData);
	void AreaDamage(ESkillKeyType KeyType, UDA_SkillBase* SkillData);
	void Buff(ESkillKeyType KeyType, UDA_SkillBase* SkillData);
	void CC(ESkillKeyType KeyType, UDA_SkillBase* SkillData);
	void Toggle(ESkillKeyType KeyType, UDA_SkillBase* SkillData);
	void Passive(ESkillKeyType KeyType, UDA_SkillBase* SkillData);

	// ───────── 전술 스킬 실행 함수 (TacticalSkill.SkillStat 기반, 11종) ─────────
	void Blink();
	void Quake();
	void ProtocolViolation();
	void ElectricShift();
	void ForceField();
	void Totem();
	void Nullification();
	void SoulStealer();
	void TheStrijder();
	void BladeofTruth();
	void HealingWind();

	// ───────── 무기 스킬 실행 함수 (WeaponType 기반, 현재 2종만 구현) ─────────
	void Bow();
	void TwoHandedSword();

	// ───────── 쿨다운 처리 (Map 제거, 충돌 없는 Delegate 바인딩 방식) ─────────

	// 쿨다운 시작: 슬롯 인덱스(KeyType)를 타이머 콜백에 바로 바인딩하여 변수 공유 충돌을 없앤다.
	void StartCooldown(ESkillKeyType KeyType, float Cooldown);

	// 타이머 만료 시 호출됨. KeyType은 CreateUObject에서 바인딩되어 전달되므로
	// 다른 스킬이 동시에 쿨다운 중이어도 서로 덮어쓰지 않는다.
	void HandleCooldownFinished(ESkillKeyType KeyType);

	bool IsSkillReady(ESkillKeyType KeyType) const;

	// 슬롯별 타이머 핸들 (Map 대신 고정 슬롯 6개로 관리해도 되지만,
	// 기존 구조(Map<KeyType, TimerHandle>)를 그대로 유지)
	UPROPERTY()
	TMap<ESkillKeyType, FTimerHandle> CooldownTimerHandles;

	// 슬롯별 사용 가능 여부 (Map 유지, 단 Set 시점에 KeyType을 변수에 따로 보관하지 않음)
	UPROPERTY(BlueprintReadOnly, Category = "Skill")
	TMap<ESkillKeyType, bool> bCanUseSkill;

public:
	// ───────── Delegates ─────────

	UPROPERTY(BlueprintAssignable, Category = "Skill|Delegate")
	FOnSkillExecuted OnSkillExecuted;

	UPROPERTY(BlueprintAssignable, Category = "Skill|Delegate")
	FOnCooldownFinished OnCooldownFinished;
};