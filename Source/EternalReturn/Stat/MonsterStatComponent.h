#pragma once
#include "CoreMinimal.h"
#include "Stat/BaseStatComponent.h"
#include "ERTypes.h"
#include "MonsterStatComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ETERNALRETURN_API UMonsterStatComponent : public UBaseStatComponent
{
    GENERATED_BODY()

public:
    UMonsterStatComponent();
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
    // =================== AI 전용 (복제 불필요) ===================

    float AggroRange = 500.f;

    float ChaseRange = 1000.f;

    // =================== 드롭 보상 ===================

    float DropGold = 0.f;

    float DropExp = 0.f;

    // =================== 몬스터 정보 ===================

    EMonsterType MonsterType;

    float RespawnTime = 0.f;

public:
    // =================== Setter ===================

    UFUNCTION(BlueprintCallable, Category = "Stat")
    void SetAggroRange(float Value) { AggroRange = Value; }

    UFUNCTION(BlueprintCallable, Category = "Stat")
    void SetChaseRange(float Value) { ChaseRange = Value; }

    UFUNCTION(BlueprintCallable, Category = "Stat")
    void SetDropGold(float Value) { DropGold = Value; }

    UFUNCTION(BlueprintCallable, Category = "Stat")
    void SetDropExp(float Value) { DropExp = Value; }

    UFUNCTION(BlueprintCallable, Category = "Stat")
    void SetMonsterType(EMonsterType Value) { MonsterType = Value; }

    UFUNCTION(BlueprintCallable, Category = "Stat")
    void SetRespawnTime(float Value) { RespawnTime = Value; }

    // =================== Getter ===================

    UFUNCTION(BlueprintPure, Category = "Stat")
    float GetAggroRange() const { return AggroRange; }

    UFUNCTION(BlueprintPure, Category = "Stat")
    float GetChaseRange() const { return ChaseRange; }

    UFUNCTION(BlueprintPure, Category = "Stat")
    float GetDropGold() const { return DropGold; }

    UFUNCTION(BlueprintPure, Category = "Stat")
    float GetDropExp() const { return DropExp; }

    UFUNCTION(BlueprintPure, Category = "Stat")
    EMonsterType GetMonsterType() const { return MonsterType; }

    UFUNCTION(BlueprintPure, Category = "Stat")
    float GetRespawnTime() const { return RespawnTime; }

    // =================== DataTable 초기화 ===================

    UFUNCTION(BlueprintCallable, Category = "Stat")
    void InitFromDataTable(FName RowName);
};