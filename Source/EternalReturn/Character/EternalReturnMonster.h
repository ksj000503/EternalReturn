#pragma once

#include "CoreMinimal.h"
#include "CombatEntityBase.h"
#include "Stat/MonsterStatComponent.h"
#include "EternalReturnMonster.generated.h"

UCLASS(abstract)
class ETERNALRETURN_API AEternalReturnMonster : public ACombatEntityBase
{
    GENERATED_BODY()

public:
    AEternalReturnMonster();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")

    TObjectPtr<UMonsterStatComponent> MonsterStatComponent;
};